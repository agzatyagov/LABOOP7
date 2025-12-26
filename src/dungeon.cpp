#include "dungeon.h"
#include "visitor.h"
#include <deque>
#include <cmath>

static int Clamp(int v, int lo, int hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static int RollDice(std::mt19937& rng)
{
    std::uniform_int_distribution<int> d(1, 6);//генерация случайных чисел от 1 до 6
    return d(rng);
}

static char TypeChar(NPCType t)//отображение на карте
{
    if (t == NPCType::SlaveTrader) return 'T';
    if (t == NPCType::Squirrel) return 'S';
    if (t == NPCType::WanderingKnight) return 'K';
    return '?';
}

static std::string MakeKillMessage(const std::shared_ptr<NPC>& a, const std::shared_ptr<NPC>& b)
{
    std::string msg;
    msg = TypeToString(a->getType()) + " " + a->getName() + " killed " +TypeToString(b->getType()) + " " + b->getName();
    return msg;
}

Dungeon::Dungeon()
{
}

bool Dungeon::nameExistsNoLock(const std::string& name) const//отсутствие одинаковых имён
{
    size_t i = 0;
    while (i < npcs.size())
    {
        if (npcs[i]->getName() == name) return true;
        i = i + 1;
    }
    return false;
}

void Dungeon::createRandomNPCs(int count, NPCFactory& factory, unsigned seed)//создание рандомных нпс
{
    std::mt19937 rng(seed);

    std::uniform_int_distribution<int> typeDist(1, 3);
    std::uniform_int_distribution<int> xDist(0, MAP_W - 1);
    std::uniform_int_distribution<int> yDist(0, MAP_H - 1);

    std::lock_guard<std::shared_mutex> lg(mtx);

    npcs.clear();
    npcs.reserve((size_t)count);

    for (int i = 0; i < count; i = i + 1)
    {
        NPCType type = (NPCType)typeDist(rng);

        int x = xDist(rng);
        int y = yDist(rng);

        std::string name;
        name = std::string(1, TypeChar(type)) + std::to_string(i);

        std::shared_ptr<NPC> npc = factory.create(type, name, x, y);
        if (npc.get() != 0)
        {
            npcs.push_back(npc);
        }
    }
}

void Dungeon::moveAndCollectTasks(std::vector<BattleTask>& outTasks, std::mt19937& rng)
{
    outTasks.clear();

    std::uniform_int_distribution<int> dirDist(0, 3);

    std::lock_guard<std::shared_mutex> lg(mtx);//

    //1) движение
    size_t i = 0;
    while (i < npcs.size())
    {
        if (npcs[i]->isAlive())//движение осуществляется только живыми
        {
            int step = npcs[i]->getMoveDistance();
            int dir = dirDist(rng);

            int nx = npcs[i]->getX();
            int ny = npcs[i]->getY();

            if (dir == 0) nx = nx + step;//право
            if (dir == 1) nx = nx - step;//лево
            if (dir == 2) ny = ny + step;//вниз
            if (dir == 3) ny = ny - step;//вверх

            nx = Clamp(nx, 0, MAP_W - 1);//защита от выхода за карту по x
            ny = Clamp(ny, 0, MAP_H - 1);//по y

            npcs[i]->setPos(nx, ny);
        }

        i = i + 1;
    }

    //2)поиск боев: только если attacker реально может убить defender и в своём радиусе
    size_t a = 0;
    while (a < npcs.size())
    {
        if (!npcs[a]->isAlive())
        {
            a = a + 1;
            continue;
        }

        int ar = npcs[a]->getKillDistance();
        int ar2 = ar * ar;

        size_t b = 0;
        while (b < npcs.size())
        {
            if (a != b && npcs[b]->isAlive())
            {
                NPCType at = npcs[a]->getType();

                TypeVisitor tv;
                npcs[b]->accept(tv);
                NPCType dt = tv.type;

                if (CanKill(at, dt))
                {
                    int dx = npcs[a]->getX() - npcs[b]->getX();
                    int dy = npcs[a]->getY() - npcs[b]->getY();
                    int d2 = dx * dx + dy * dy;

                    if (d2 <= ar2)
                    {
                        BattleTask task;
                        task.attacker = npcs[a];
                        task.defender = npcs[b];
                        outTasks.push_back(task);
                    }
                }
            }

            b = b + 1;
        }

        a = a + 1;
    }
}

void Dungeon::processTask(const BattleTask& task, BattleSubject& subject, std::mt19937& rng)
{
    if (task.attacker.get() == 0 || task.defender.get() == 0) return;

    std::lock_guard<std::shared_mutex> lg(mtx);//защищ.сост alive

    if (!task.attacker->isAlive()) return;
    if (!task.defender->isAlive()) return;

    NPCType at = task.attacker->getType();

    TypeVisitor tv;
    task.defender->accept(tv);
    NPCType dt = tv.type;

    if (!CanKill(at, dt)) return;

    int r = task.attacker->getKillDistance();
    int r2 = r * r;

    int dx = task.attacker->getX() - task.defender->getX();
    int dy = task.attacker->getY() - task.defender->getY();
    int d2 = dx * dx + dy * dy;

    if (d2 > r2) return;

    //бой: attacker кидает "атаку", defender кидает "защиту"
    int attack = RollDice(rng);
    int defense = RollDice(rng);

    if (attack > defense)
    {
        task.defender->kill();
        subject.notifyKill(MakeKillMessage(task.attacker, task.defender));
    }
}

void Dungeon::buildMap(std::vector<std::string>& outMap) const
{
    outMap.clear();
    outMap.resize(MAP_H);

    int y = 0;
    while (y < MAP_H)
    {
        outMap[y] = std::string(MAP_W, '.');
        y = y + 1;
    }

    std::shared_lock<std::shared_mutex> sl(mtx);
    size_t i = 0;
    while (i < npcs.size())
    {
        if (npcs[i]->isAlive())
        {
            int x = npcs[i]->getX();
            int yy = npcs[i]->getY();

            if (x >= 0 && x < MAP_W && yy >= 0 && yy < MAP_H)
            {
                outMap[yy][x] = TypeChar(npcs[i]->getType());
            }
        }
        i = i + 1;
    }
}

void Dungeon::getSurvivors(std::vector<std::shared_ptr<NPC>>& out) const
{
    out.clear();

    std::shared_lock<std::shared_mutex> sl(mtx);

    size_t i = 0;
    while (i < npcs.size())
    {
        if (npcs[i]->isAlive())
        {
            out.push_back(npcs[i]);
        }
        i = i + 1;
    }
}

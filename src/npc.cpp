#include "npc.h"
#include "visitor.h"

NPC::NPC(const std::string& name_, int x_, int y_)
{
    name = name_;
    x = x_;
    y = y_;
    alive = true;
}

std::string NPC::getName() const { return name; }
int NPC::getX() const { return x; }
int NPC::getY() const { return y; }

bool NPC::isAlive() const { return alive; }

void NPC::kill()
{
    alive = false;
}

void NPC::setPos(int nx, int ny)
{
    x = nx;
    y = ny;
}

int NPC::getMoveDistance() const
{
    NPCType t = getType();
    if (t == NPCType::SlaveTrader) return 10;
    if (t == NPCType::Squirrel) return 5;
    if (t == NPCType::WanderingKnight) return 30;
    return 0;
}

int NPC::getKillDistance() const
{
    NPCType t = getType();
    if (t == NPCType::SlaveTrader) return 10;
    if (t == NPCType::Squirrel) return 5;
    if (t == NPCType::WanderingKnight) return 10;
    return 0;
}

SlaveTrader::SlaveTrader(const std::string& name_, int x_, int y_) : NPC(name_, x_, y_) {}
NPCType SlaveTrader::getType() const { return NPCType::SlaveTrader; }
void SlaveTrader::accept(NPCVisitor& v) { v.visit(*this); }

Squirrel::Squirrel(const std::string& name_, int x_, int y_) : NPC(name_, x_, y_) {}
NPCType Squirrel::getType() const { return NPCType::Squirrel; }
void Squirrel::accept(NPCVisitor& v) { v.visit(*this); }

WanderingKnight::WanderingKnight(const std::string& name_, int x_, int y_) : NPC(name_, x_, y_) {}
NPCType WanderingKnight::getType() const { return NPCType::WanderingKnight; }
void WanderingKnight::accept(NPCVisitor& v) { v.visit(*this); }

std::string TypeToString(NPCType type)
{
    if (type == NPCType::SlaveTrader) return "SlaveTrader";
    if (type == NPCType::Squirrel) return "Squirrel";
    if (type == NPCType::WanderingKnight) return "WanderingKnight";
    return "Unknown";
}

NPCType StringToType(const std::string& s)
{
    if (s == "SlaveTrader") return NPCType::SlaveTrader;
    if (s == "Squirrel") return NPCType::Squirrel;
    if (s == "WanderingKnight") return NPCType::WanderingKnight;
    return NPCType::Unknown;
}

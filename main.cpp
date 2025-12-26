#include <thread>
#include <atomic>
#include <deque>
#include <vector>
#include <chrono>
#include <mutex>
#include <iostream>
#include "dungeon.h"
#include "npc_factory.h"
#include "observer.h"

int main()
{
    Dungeon dungeon;
    NPCFactory factory;

    BattleSubject subject;
    ConsoleObserver consoleObs;
    FileObserver fileObs("log.txt");
    subject.attach(&consoleObs);
    subject.attach(&fileObs);

    // старт: 50 NPC в случайных местах
    dungeon.createRandomNPCs(50, factory, 12345);

    std::deque<BattleTask> queue;
    std::mutex queueMtx;

    std::atomic<bool> running(true);

    //поток 1: движение + поиск дистанции убийства + постановка задач
    std::thread mover([&]()
        {
            std::mt19937 rng(111);

            while (running.load())
            {
                std::vector<BattleTask> tasks;
                dungeon.moveAndCollectTasks(tasks, rng);

                {
                    std::lock_guard<std::mutex> lg(queueMtx);
                    size_t i = 0;
                    while (i < tasks.size())
                    {
                        queue.push_back(tasks[i]);
                        i = i + 1;
                    }

                    //чтобы очередь не разрасталась бесконечно
                    while (queue.size() > 5000)
                    {
                        queue.pop_front();
                    }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
        }); 

    //Поток 2: бои
    std::thread fighter([&]()
        {
            std::mt19937 rng(222);

            while (running.load() || true)
            {
                BattleTask task;
                bool has = false;

                {
                    std::lock_guard<std::mutex> lg(queueMtx);
                    if (!queue.empty())
                    {
                        task = queue.front();
                        queue.pop_front();
                        has = true;
                    }
                }

                if (has)
                {
                    dungeon.processTask(task, subject, rng);
                    continue;
                }

                if (!running.load())
                {
                    // если игра остановлена и очередь пуста — выходим
                    std::lock_guard<std::mutex> lg(queueMtx);
                    if (queue.empty()) break;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });

    //главный, печатающий каждые 30 сек
    auto start = std::chrono::steady_clock::now();

    while (true)
    {
        auto now = std::chrono::steady_clock::now();
        auto sec = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();

        if (sec >= 30)
        {
            running.store(false);
            break;
        }

        std::vector<std::string> map;
        dungeon.buildMap(map);

        {
            std::lock_guard<std::mutex> out(GetCoutMutex());
            std::cout << "\nTime: " << sec << " sec\n";
            for (size_t i = 0; i < map.size(); i = i + 1)
            {
                std::cout << map[i] << "\n";
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    mover.join();
    fighter.join();

    //вывод выживших
    std::vector<std::shared_ptr<NPC>> survivors;
    dungeon.getSurvivors(survivors);

    {
        std::lock_guard<std::mutex> out(GetCoutMutex());
        std::cout << "\n=== Survivors (" << survivors.size() << ") ===\n";
        for (size_t i = 0; i < survivors.size(); i = i + 1)
        {
            std::cout << TypeToString(survivors[i]->getType()) << " "
                << survivors[i]->getName() << " "
                << "x=" << survivors[i]->getX() << " "
                << "y=" << survivors[i]->getY() << "\n";
        }
    }

    return 0;
}

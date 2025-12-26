#pragma once
#include <vector>
#include <memory>
#include <string>
#include <shared_mutex>
#include <random>

#include "npc.h"
#include "npc_factory.h"
#include "observer.h"

struct BattleTask
{
    std::shared_ptr<NPC> attacker;
    std::shared_ptr<NPC> defender;
};

class Dungeon
{
public:
    static const int MAP_W = 30;
    static const int MAP_H = 30;

private:
    std::vector<std::shared_ptr<NPC>> npcs;
    mutable std::shared_mutex mtx;//т.к много чтений(печатей карты)

    bool nameExistsNoLock(const std::string& name) const;

public:
    Dungeon();

    void createRandomNPCs(int count, NPCFactory& factory, unsigned seed);

    //поток движения: двигает живых и собирает задачи боя 
    void moveAndCollectTasks(std::vector<BattleTask>& outTasks, std::mt19937& rng);

    //поток боя
    void processTask(const BattleTask& task, BattleSubject& subject, std::mt19937& rng);

    //главный поток: снимок карты для печати
    void buildMap(std::vector<std::string>& outMap) const;

    // список выживших в конце
    void getSurvivors(std::vector<std::shared_ptr<NPC>>& out) const;
};

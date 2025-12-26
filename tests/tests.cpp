#include <gtest/gtest.h>
#include <random>
#include <vector>
#include "npc_factory.h"
#include "visitor.h"
#include "dungeon.h"

TEST(FactoryTest, Create)
{
    NPCFactory f;

    auto a = f.create(NPCType::SlaveTrader, "T0", 1, 2);
    auto b = f.create(NPCType::Squirrel, "S0", 3, 4);
    auto c = f.create(NPCType::WanderingKnight, "K0", 5, 6);

    ASSERT_TRUE(a.get() != nullptr);
    ASSERT_TRUE(b.get() != nullptr);
    ASSERT_TRUE(c.get() != nullptr);

    EXPECT_EQ(a->getType(), NPCType::SlaveTrader);
    EXPECT_EQ(b->getType(), NPCType::Squirrel);
    EXPECT_EQ(c->getType(), NPCType::WanderingKnight);
}

TEST(KillTableTest, CanKillRules)
{
    EXPECT_TRUE(CanKill(NPCType::SlaveTrader, NPCType::Squirrel));
    EXPECT_TRUE(CanKill(NPCType::WanderingKnight, NPCType::SlaveTrader));
    EXPECT_TRUE(CanKill(NPCType::Squirrel, NPCType::Squirrel));

    EXPECT_FALSE(CanKill(NPCType::SlaveTrader, NPCType::SlaveTrader));
    EXPECT_FALSE(CanKill(NPCType::SlaveTrader, NPCType::WanderingKnight));
    EXPECT_FALSE(CanKill(NPCType::WanderingKnight, NPCType::Squirrel));
}

TEST(DungeonTest, MapBuildDoesNotCrash)
{
    Dungeon d;
    NPCFactory f;

    d.createRandomNPCs(50, f, 123);

    std::vector<std::string> map;
    d.buildMap(map);

    ASSERT_EQ((int)map.size(), Dungeon::MAP_H);
    EXPECT_EQ((int)map[0].size(), Dungeon::MAP_W);
}

TEST(NPCTest, DistancesFromTable)
{
    NPCFactory f;
    auto t = f.create(NPCType::SlaveTrader, "T", 0, 0);
    auto s = f.create(NPCType::Squirrel, "S", 0, 0);
    auto k = f.create(NPCType::WanderingKnight, "K", 0, 0);

    ASSERT_TRUE(t.get() && s.get() && k.get());

    EXPECT_EQ(t->getMoveDistance(), 10);
    EXPECT_EQ(t->getKillDistance(), 10);

    EXPECT_EQ(s->getMoveDistance(), 5);
    EXPECT_EQ(s->getKillDistance(), 5);

    EXPECT_EQ(k->getMoveDistance(), 30);
    EXPECT_EQ(k->getKillDistance(), 10);
}

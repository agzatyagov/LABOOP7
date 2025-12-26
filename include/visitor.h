#pragma once
#include "npc.h"

class NPCVisitor
{
public:
    virtual ~NPCVisitor() {}
    virtual void visit(SlaveTrader& npc) = 0;
    virtual void visit(Squirrel& npc) = 0;
    virtual void visit(WanderingKnight& npc) = 0;
};

//таблица убиваемости
bool CanKill(NPCType attacker, NPCType defender);

class TypeVisitor : public NPCVisitor
{
public:
    NPCType type;

    TypeVisitor();

    void visit(SlaveTrader& npc) override;
    void visit(Squirrel& npc) override;
    void visit(WanderingKnight& npc) override;
};

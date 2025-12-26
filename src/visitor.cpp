#include "visitor.h"

bool CanKill(NPCType attacker, NPCType defender)
{
    if (attacker == NPCType::SlaveTrader && defender == NPCType::Squirrel) return true;
    if (attacker == NPCType::WanderingKnight && defender == NPCType::SlaveTrader) return true;
    if (attacker == NPCType::Squirrel && defender == NPCType::Squirrel) return true;
    return false;
}

TypeVisitor::TypeVisitor()
{
    type = NPCType::Unknown;
}

void TypeVisitor::visit(SlaveTrader& npc)
{
    (void)npc;
    type = NPCType::SlaveTrader;
}

void TypeVisitor::visit(Squirrel& npc)
{
    (void)npc;
    type = NPCType::Squirrel;
}

void TypeVisitor::visit(WanderingKnight& npc)
{
    (void)npc;
    type = NPCType::WanderingKnight;
}

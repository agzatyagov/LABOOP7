#include "npc_factory.h"

std::shared_ptr<NPC> NPCFactory::create(NPCType type, const std::string& name, int x, int y)
{
    if (type == NPCType::SlaveTrader) return std::make_shared<SlaveTrader>(name, x, y);
    if (type == NPCType::Squirrel) return std::make_shared<Squirrel>(name, x, y);
    if (type == NPCType::WanderingKnight) return std::make_shared<WanderingKnight>(name, x, y);
    return std::shared_ptr<NPC>();
}

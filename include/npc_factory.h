#pragma once
#include <memory>
#include <string>
#include "npc.h"

class NPCFactory
{
public:
    std::shared_ptr<NPC> create(NPCType type, const std::string& name, int x, int y);
};

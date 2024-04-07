#pragma once
#include <string>
#include <memory>
#include <asapp/interfaces/components/tooltip.h>

namespace llpp::bots::drops
{
    struct LootResult final
    {
    public:
        std::string name;
        bool looted{false};
        std::optional<asa::interfaces::components::ItemTooltip> tooltip;
    };
}


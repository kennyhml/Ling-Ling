#pragma once
#include <chrono>
#include <string>
#include <asapp/items/items.h>
#include <asapp/structures/craftingstation.h>
#include "../../core/basestation.h"
#include "basepowderstation.h"

namespace llpp::bots::crafting
{
    class SparkpowderStation final : public BasePowderStation
    {
    public:
        SparkpowderStation(const std::string& t_name,
                           const std::chrono::minutes t_interval) : BasePowderStation(
            t_name, t_interval, *asa::items::resources::sparkpowder, 800) {}

        core::StationResult complete() override;
    };
}

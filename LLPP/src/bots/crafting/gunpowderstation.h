#pragma once
#include <asapp/items/items.h>
#include "basepowderstation.h"
#include "../../core/basestation.h"


namespace llpp::bots::crafting
{
    class GunpowderStation final : public BasePowderStation
    {
    public:
        GunpowderStation(const std::string& t_name,
                         const std::chrono::minutes t_interval) : BasePowderStation(
            t_name, t_interval, *asa::items::resources::gunpowder, 1000) {}

        core::StationResult complete() override;
    };
}

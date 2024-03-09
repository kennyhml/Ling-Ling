#pragma once
#include "cratestation.h"


namespace llpp::bots::drops
{
    class TeleportCrateStation final : public core::TeleportStation, public CrateStation
    {
    public:
        TeleportCrateStation(const std::string& t_name, CrateManagerConfig* t_config,
                        asa::structures::CaveLootCrate t_crate, bool t_is_first,
                        bool t_is_first_in_group);

        core::StationResult complete() override;
    };
}

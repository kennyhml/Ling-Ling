#pragma once
#include "../../../core/teleportstation.h"

namespace llpp::bots::farm
{
    class WoodStation final : public core::TeleportStation
    {
    public:
        explicit WoodStation(const std::string& t_name);

        core::StationResult complete() override;
    };
}

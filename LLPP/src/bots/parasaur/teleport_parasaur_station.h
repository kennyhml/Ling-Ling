#pragma once
#include "config.h"
#include "../../core/teleportstation.h"
#include "../../core/bedstation.h"
#include <asapp/entities/dinoent.h>

namespace llpp::bots::parasaur
{
    class TeleportParasaurStation final : public core::TeleportStation
    {
    public:
        TeleportParasaurStation(std::string t_real_name, ParasaurConfig* t_config);

        core::StationResult complete() override;

    private:
        static bool has_lingering_ping();

        ParasaurConfig* config_;

        inline static std::chrono::system_clock::time_point last_detected_;

        std::string real_name_;
        asa::entities::DinoEnt parasaur_;
    };
}

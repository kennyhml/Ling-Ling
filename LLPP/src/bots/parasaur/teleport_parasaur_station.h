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

        bool is_ready() override
        {
            return !config_->disabled && TeleportStation::is_ready();
        }

        core::StationResult complete() override;

        /**
         * @return Gets the time point of the last enemy detection at this station.
         */
        [[nodiscard]] std::chrono::system_clock::time_point get_last_alert() const
        {
            return last_detection_;
        };

    private:
        static bool has_lingering_ping();

        ParasaurConfig* config_;

        inline static std::chrono::system_clock::time_point last_detected_;

        std::chrono::system_clock::time_point last_detection_;
        std::string real_name_;
        asa::entities::DinoEnt parasaur_;
    };
}

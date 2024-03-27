#pragma once
#include <asapp/entities/localplayer.h>
#include "../../core/teleportstation.h"


namespace llpp::bots::metal
{

    class MetalStation final : core::TeleportStation
    {
    public:
        using TeleportStation::TeleportStation;

        core::StationResult complete() override;

    private:
        /**
         * @brief Swings the amount of times previously determined by count_swings.
         */
        void swing();

        static int count_swings();

        int swing_times_ = 0;

        inline static asa::entities::DinoEntity anky{"METAL ANKY"};
        inline static std::chrono::system_clock::time_point last_log_check;
    };


}





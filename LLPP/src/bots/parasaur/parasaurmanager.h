#pragma once
#include "teleport_parasaur_station.h"
#include "bed_parasaur_station.h"
#include "../../core/istationmanager.h"

namespace llpp::bots::parasaur
{
    class ParasaurManager final : public core::IStationManager
    {
    public:
        ParasaurManager();

        bool run() override;
        [[nodiscard]] bool is_ready_to_run() override;
        [[nodiscard]] std::chrono::minutes get_time_left_until_ready() const override;
    private:
        /**
         * @brief Travels to the start tp to start the tp stations.
         */
        bool go_to_start();

        std::chrono::system_clock::time_point last_completed_;

        std::vector<std::unique_ptr<BedParasaurStation>> bed_stations_;
        std::vector<std::unique_ptr<TeleportParasaurStation>> tp_stations_;
        core::BedStation spawn_bed_;
        core::TeleportStation spawn_tp_;
        core::TeleportStation next_tp_;
    };
}

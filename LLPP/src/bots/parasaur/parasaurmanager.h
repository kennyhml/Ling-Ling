#pragma once
#include "teleport_parasaur_station.h"
#include "bed_parasaur_station.h"
#include "../../core/istationmanager.h"

#define UNDEFINED_TIME std::chrono::system_clock::time_point()

namespace llpp::bots::parasaur
{
    class ParasaurManager final : public core::IStationManager
    {
    public:
        ParasaurManager();

        bool run() override;
        [[nodiscard]] bool is_ready_to_run() override;
        [[nodiscard]] std::chrono::minutes get_time_left_until_ready() const override;

        struct ParasaurGroupStatistics {
        public:
            std::string parasaur_station_name;
            bool parasaur_alerting = false;
            int times_checked_ = 0;
            std::chrono::system_clock::time_point last_checked = UNDEFINED_TIME;

            void station_checked(const std::string& stationname, bool alerting = false);
        };

        std::vector<ParasaurGroupStatistics> station_stats;

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

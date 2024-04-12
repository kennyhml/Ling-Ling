#pragma once
#include "config.h"
#include "cratestation.h"
#include "bed_crate_station.h"
#include "teleport_crate_station.h"
#include "../../core/istationmanager.h"
#include <dpp/dpp.h>

#include "dropoffstation.h"

namespace llpp::bots::drops
{
    using QualityFlags = int;

    class CrateManager final : public core::IStationManager
    {
    public:
        explicit CrateManager(CrateManagerConfig* t_config);

        bool run() override;

        [[nodiscard]] bool is_ready_to_run() override;
        [[nodiscard]] std::chrono::minutes get_time_left_until_ready() const override;

    public:
        struct CrateGroupStatistics
        {
        public:
            std::chrono::system_clock::time_point last_looted;

            void add_looted();

            [[nodiscard]] int get_times_looted() const { return times_looted_; }
            [[nodiscard]] std::chrono::seconds get_avg() const { return avg_respawn_; }

        private:
            int times_looted_ = 0;
            std::chrono::seconds avg_respawn_ = std::chrono::seconds(0);
        };

        std::vector<CrateGroupStatistics> stats_per_group;

    private:
        CrateManagerConfig* config_;

        std::vector<std::vector<BedCrateStation>> bed_stations_;
        std::vector<std::vector<TeleportCrateStation>> teleport_stations_;

        core::BedStation teleport_align_station_;
        core::BedStation beds_out_station_;
        core::TeleportStation teleport_dropoff_station_;

        asa::structures::Container dropoff_vault_;
        std::map<std::string, float> vaults_filled_;

        inline static bool has_registered_reroll_command_ = false;

        inline static DropoffStation dropoff_;

    private:
        static void reroll_mode_callback(const dpp::slashcommand_t&, void*);

        void run_teleport_stations();

        void run_bed_stations();

        void spawn_on_align_bed();
        void register_slash_commands();
    };
}

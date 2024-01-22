#pragma once
#include "../../core/istationmanager.h"
#include "config.h"
#include "lootcratestation.h"
#include <asapp/structures/simplebed.h>
#include "../../core/discord.h"

#define UNDEFINED_TIME std::chrono::system_clock::time_point()

namespace llpp::bots::drops
{
    using QualityFlags = int;

    /**
     * @brief Handles the completion of multiple related crate stations.
     *
     *
     *
     */
    class CrateManager final : public core::IStationManager
    {
    public:
        explicit CrateManager(CrateManagerConfig& t_config);

        bool run() override;
        bool is_disabled() const { return config_.disabled; }

        [[nodiscard]] bool is_ready_to_run() const override;
        [[nodiscard]] std::chrono::minutes get_time_left_until_ready() const override;

    public:
        struct CrateGroupStatistics
        {
        public:
            std::chrono::system_clock::time_point last_looted = UNDEFINED_TIME;

            void add_looted();

            [[nodiscard]] int get_times_looted() const { return times_looted_; }
            [[nodiscard]] std::chrono::seconds get_avg() const { return avg_respawn_; }

        private:
            int times_looted_ = 0;
            std::chrono::seconds avg_respawn_ = std::chrono::seconds(0);
        };

        std::vector<CrateGroupStatistics> stats_per_group;
        void parse_groups(std::string groups);

    private:
        CrateManagerConfig& config_;
        std::vector<std::vector<LootCrateStation>> crates_;

        asa::structures::SimpleBed align_bed_;
        asa::structures::SimpleBed out_bed_;

        asa::structures::Teleporter dropoff_tp_;
        asa::structures::Container dropoff_vault_;

        std::map<std::string, float> vault_fill_levels_;

        inline static bool has_registered_reroll_command_ = false;

    private:
        static void reroll_mode_callback(const dpp::slashcommand_t&);

        void dropoff_items();
        void teleport_to_dropoff();
        void run_all_stations(bool& any_looted);
        void spawn_on_align_bed();
        void register_slash_commands();
    };
}

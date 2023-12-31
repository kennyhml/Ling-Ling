#pragma once
#include "../../core/istationmanager.h"
#include "../suicide/suicidestation.h"
#include "lootcratestation.h"
#include <asapp/structures/simplebed.h>
#include <dpp/dpp.h>

#include "parameters.h"

#define UNDEFINED_TIME std::chrono::system_clock::time_point::min()

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
        CrateManager(const CrateStationParams& t_params,
                     const std::vector<std::vector<QualityFlags>>& t_grouped_crates,
                     suicide::SuicideStation* t_suicide = nullptr);

        bool run() override;
        [[nodiscard]] bool is_ready_to_run() const override;
        [[nodiscard]] std::chrono::minutes get_time_left_until_ready() const override;

    public:
        struct CrateGroupStatistics
        {
        public:
            std::chrono::system_clock::time_point last_looted = UNDEFINED_TIME;

            void add_looted();

            [[nodiscard ]] int get_times_looted() const { return times_looted_; }
            [[nodiscard ]] std::chrono::seconds get_avg() const { return avg_respawn_; }

        private:
            int times_looted_ = 0;
            std::chrono::seconds avg_respawn_ = std::chrono::seconds(0);
        };

        std::vector<CrateGroupStatistics> stats_per_group;

        // static core::data::ManagedVar<bool> get_reroll_mode();

    private:
        const std::string prefix_;
        const bool use_beds_;
        const std::chrono::seconds allowed_render_time_;

        asa::structures::SimpleBed align_bed_;
        asa::structures::Teleporter dropoff_tp_;
        asa::structures::Container dropoff_vault_;

        suicide::SuicideStation* suicide_;

        float last_known_vault_fill_level_ = 0.f;
        inline static bool has_registered_reroll_command_ = false;
        std::vector<std::vector<LootCrateStation>> crates_;

    private:
        static void reroll_mode_callback(const dpp::slashcommand_t&);

        void dropoff_items(float& fill_level_out);
        void teleport_to_dropoff();
        void run_all_stations(bool& any_looted);
        void spawn_on_align_bed();
        void register_slash_commands();
        void populate_groups(const std::vector<std::vector<QualityFlags>>& groups,
                             std::chrono::minutes interval);
    };
}

#pragma once
#include "../../core/basestation.h"
#include <asapp/structures/cavelootcrate.h>
#include <asapp/structures/teleporter.h>
#include <asapp/structures/simplebed.h>

#include "config.h"

namespace llpp::bots::drops
{
    class LootCrateStation final : public core::BaseStation
    {
    public:
        explicit LootCrateStation(const std::string& t_name, CrateManagerConfig& t_config,
                                  asa::structures::CaveLootCrate t_crate);

        core::StationResult complete() override;

        void set_can_default_teleport(bool can_default_teleport);
        void set_cooldown();
        void set_fully_loot(bool fully_loot);

        int get_times_looted() const { return times_looted_; }

    private:
        bool fully_loot_ = true;
        CrateManagerConfig& config_;

        std::map<std::string, bool> cherry_pick_items() const;
        void loot_crate(cv::Mat& screenshot_out,
                        std::map<std::string, bool>& cherry_picked_out);
        void go_to();
        bool has_buff_wait_expired() const;

        bool can_default_teleport_ = false;
        int times_looted_ = 0;

        bool is_crate_up_ = false;
        std::chrono::system_clock::time_point last_discovered_up_;
        std::chrono::minutes max_buff_wait_time_ = std::chrono::minutes(15);

        asa::structures::CaveLootCrate crate_;
        asa::structures::Teleporter teleporter_;
        asa::structures::SimpleBed bed_;
        asa::structures::Container vault_;
    };
}

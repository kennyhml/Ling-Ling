#pragma once
#include "../../core/teleportstation.h"
#include "../../core/bedstation.h"
#include "config.h"
#include <asapp/structures/cavelootcrate.h>
#include <asapp/structures/simplebed.h>

namespace llpp::bots::drops
{
    class LootCrateStation final : public core::TeleportStation, public core::BedStation
    {
    public:
        explicit LootCrateStation(const std::string& t_name, CrateManagerConfig& t_config,
                                  asa::structures::CaveLootCrate t_crate, bool t_is_first,
                                  bool t_is_first_in_group);

        core::StationResult complete() override;

        void set_cooldown()
        {
            BedStation::last_completed_ = std::chrono::system_clock::now();
        }

        void set_rendered(const bool rendered) { is_rendered_ = rendered; }

        [[nodiscard]] int get_times_looted() const { return times_looted_; }
        [[nodiscard]] int get_vault_slots() const { return vault_slots_; }

        const asa::structures::Container& get_vault() const { return vault_; }

    private:
        CrateManagerConfig& config_;

        std::map<std::string, bool> cherry_pick_items();
        std::vector<std::string> last_seen_items_;

        void loot_crate(cv::Mat& screenshot_out,
                        std::map<std::string, bool>& cherry_picked_out);
        bool has_buff_wait_expired() const;
        bool should_reroll() const;
        int times_looted_ = 0;
        int vault_slots_ = 0;

        bool is_first_;
        bool is_first_in_group_;
        bool is_crate_up_ = false;
        bool is_rendered_ = false;
        bool got_rerolled_ = false;

        std::chrono::system_clock::time_point last_discovered_up_;
        std::chrono::minutes max_buff_wait_time_ = std::chrono::minutes(15);

        asa::structures::CaveLootCrate crate_;
        asa::structures::Container vault_;
    };
}

#pragma once
#include "lootresult.h"
#include "../../core/teleportstation.h"
#include "../../core/bedstation.h"
#include "config.h"
#include <asapp/structures/cavelootcrate.h>

namespace llpp::bots::drops
{
    bool any_looted(const std::vector<LootResult>& items);

    class CrateStation
    {
    public:
        /**
         * @return Gets how often the crate of this station was looted.
         */
        [[nodiscard]] int get_times_looted() const { return times_looted_; }

        /**
         * @param rendered Sets whether this station is currently rendered.
         */
        void set_rendered(const bool rendered) { is_rendered_ = rendered; }

    protected:
        CrateStation(CrateManagerConfig* t_config, asa::structures::CaveLootCrate t_crate,
                     bool t_is_first_crate, bool t_is_first_of_group);

        /**
         * @return Returns whether the time the drop has been up exceeds the limit.
         */
        [[nodiscard]] bool has_buff_wait_expired() const
        {
            return util::timedout(last_found_up_, buff_max_wait_);
        }

        /**
         * Checks whether this crate is eligible for rerolling, true if:
         * - Reroll mode is enabled
         * - Reroll mode is not overruled for this crate
         * - The time the drop was found to be up has not exceeded the limit
         *
         * @returns True if all the conditions are met, false otherwise.
         */
        [[nodiscard]] bool should_reroll() const;


        [[nodiscard]] bool await_crate_loaded() const;

        /**
         * @brief Takes the high-priority items out of the crate without despawning it.
         *
         * @return A vector where each index i represents slot i in the crate and contains
         * information about the item at that slot.
         *
         * @note REROLL RELATED FEATURE
         */
        [[nodiscard]] std::vector<LootResult> take_high_priority_items() const;

        /**
         * @brief Checks for valuable items in the drop and gets their tooltips.
         *
         * @return A vector where each index i represents slot i in the crate and contains
         * information about the item at that slot.
         *
         * @note NON-REROLL RELATED FEATURE
         */
        [[nodiscard]] std::vector<LootResult> get_valuable_tooltips() const;

        /**
         * @brief Loots the items from the crate.
         *
         * @param loot_img_out The Mat to store the image of the crate loot in.
         * @param contents A list containing each item and information if it was taken.
         */
        void loot(cv::Mat& loot_img_out, std::vector<LootResult>& contents);

        asa::structures::CaveLootCrate crate_;

        CrateManagerConfig* config_;

        bool is_first_;
        bool is_first_in_group_;

        int times_looted_{0};

        bool is_rendered_{false};
        bool is_up_{false};

        std::chrono::system_clock::time_point last_found_up_{};
        std::chrono::minutes buff_max_wait_{15};
    };
}

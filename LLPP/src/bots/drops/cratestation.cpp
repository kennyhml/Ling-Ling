#include "cratestation.h"
#include "item_priorities.h"
#include "embeds.h"
#include "../../config/config.h"
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include <opencv2/highgui.hpp>

namespace llpp::bots::drops
{
    using namespace config::bots::drops;

    namespace
    {
        std::string get_repr(const std::unique_ptr<asa::items::Item>& item)
        {
            return item ? item->info() : "???";
        }

        bool inspect_tooltip(const asa::items::ItemData& data)
        {
            return data.is_blueprint && data.has_armor_value;
        }
    }

    bool any_looted(const std::vector<LootResult>& items)
    {
        return std::ranges::any_of(
            items, [](const LootResult& res) { return res.looted; });
    }

    CrateStation::CrateStation(CrateManagerConfig* t_config,
                               asa::structures::CaveLootCrate t_crate,
                               const bool t_is_first_crate,
                               const bool t_is_first_of_group)
        : crate_(std::move(t_crate)), config_(t_config), is_first_(t_is_first_crate),
          is_first_in_group_(t_is_first_of_group) {}


    bool CrateStation::should_reroll() const
    {
        return !(has_buff_wait_expired() || config_->no_reroll) && reroll_mode.get();
    }


    bool CrateStation::await_crate_loaded() const
    {
        if (is_first_) {
            asa::core::sleep_for(std::chrono::seconds(config_->render_initial_for));
        }

        return util::await([this]() -> bool {
            return asa::entities::local_player->can_access(crate_);
        }, std::chrono::seconds(is_first_in_group_ ? config_->render_group_for : 1));
    }

    void CrateStation::loot(cv::Mat& loot_img_out, std::vector<LootResult>& taken_out)
    {
        asa::entities::local_player->access(crate_);
        // Make sure that we did not default to the crafting tab.
        crate_.get_inventory()->switch_to(asa::interfaces::CraftingInventory::INVENTORY);
        loot_img_out = asa::window::screenshot({1193, 227, 574, 200});

        // No folder being in the drop means this is a new drop or our first time
        // finding the drop, keeping track of this helps avoiding a scenario where
        // a drop respawns after being rerolled before the bot gets back to it.
        const bool has_folder = crate_.get_inventory()->slots[0].is_folder();
        if (!has_folder || !times_looted_) {
            last_found_up_ = std::chrono::system_clock::now();
        }

        if (should_reroll()) {
            // Take out the good items but leave the drop up so it can be rerolled.
            taken_out = take_high_priority_items();
            if (!has_folder) { crate_.get_inventory()->make_new_folder("checked"); }
            for (auto& item : taken_out) {
                if (item.tooltip) {
                    cv::imshow("tt", item.tooltip->get_image());
                    cv::waitKey(0);
                }
            }
            crate_.get_inventory()->close();
        }
        else {
            // Take all of the items from the drop, let it vanish so a new one can spawn.
            do { crate_.get_inventory()->transfer_all(); }
            while (!util::await([this]() -> bool {
                return crate_.get_inventory()->is_open();
            }, std::chrono::seconds(10)));
        }
        // Closing the drop or despawning it often causes a frame-drop
        asa::core::sleep_for(std::chrono::seconds(1));
    }

    std::vector<LootResult> CrateStation::take_high_priority_items() const
    {
        int taken_offset = 0;
        auto items = crate_.get_inventory()->get_current_page_items();
        std::vector<LootResult> res(items.size());

        // set all looted states to 0 initially.
        for (size_t i = 0; i < items.size(); i++) {
            res[i] = {get_repr(items[i]), false, nullptr};
        }

        // Take the items based on their priority in the priority order.
        for (int s = 0; s < items.size() && items.size() > 1; s++) {
            bool item_found = false;
            for (const auto& priority : get_priority_order()) {
                if (item_found) { break; } // restart the priority iteration
                for (size_t i = 0; i < items.size(); i++) {
                    if (!items[i] || *items[i] != priority) { continue; }

                    if (inspect_tooltip(items[i]->get_data())) {
                        const auto& slot = crate_.get_inventory()->slots[i];
                        asa::core::sleep_for(std::chrono::milliseconds(500));
                        crate_.get_inventory()->select_slot(slot);
                        res[i + taken_offset].tooltip = slot.get_tooltip();
                    }

                    crate_.get_inventory()->take_slot(static_cast<int>(i));
                    res[i + taken_offset].looted = true;
                    // erase this item from the items array to shift all the other items.
                    items.erase(items.begin() + static_cast<int>(i));
                    taken_offset++;
                    item_found = true;
                    break;
                }
            }
        }
        return res;
    }
}

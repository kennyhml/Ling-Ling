#include "cratestation.h"
#include "item_priorities.h"
#include "embeds.h"
#include "../../config/config.h"
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>

namespace llpp::bots::drops
{
    using namespace config::bots::drops;

    bool any_looted(const std::map<std::string, bool>& items)
    {
        return std::ranges::any_of(items, [](const auto& p) { return p.second; });
    }


    namespace
    {
        std::string get_repr(const std::unique_ptr<asa::items::Item>& item)
        {
            return item ? item->info() : "???";
        }
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


    void CrateStation::loot(cv::Mat& loot_img_out, std::map<std::string, bool>& taken_out)
    {
        asa::entities::local_player->access(crate_);
        // Make sure that we did not default to the crafting tab.
        crate_.get_inventory()->switch_to(asa::interfaces::CraftingInventory::INVENTORY);
        loot_img_out = asa::window::screenshot({1193, 227, 574, 200});

        // No folder being in the drop means this is a new drop or our first time
        // finding the drop, keeping track of this helps avoiding a scenario where
        // a drop respawns after being rerolled before the bot gets back to it.
        const bool has_folder = crate_.get_inventory()->slots[0].is_folder();
        if (!has_folder) { last_found_up_ = std::chrono::system_clock::now(); }

        if (should_reroll()) {
            // Take out the good items but leave the drop up so it can be rerolled.
            taken_out = take_high_priority_items();
            if (!has_folder) { crate_.get_inventory()->make_new_folder("checked"); }
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

    std::map<std::string, bool> CrateStation::take_high_priority_items() const
    {
        std::map<std::string, bool> items_taken{};
        int taken_offset = 0;
        auto items = crate_.get_inventory()->get_current_page_items();

        // set all looted states to 0 initially.
        for (size_t i = 0; i < items.size(); i++) {
            std::string repr = get_repr(items[i]);
            items_taken[std::format("{}: {}", i, repr)] = false;
        }

        // Take the items based on their priority in the priority order.
        for (int slot = 0; slot < items.size() && items.size() > 1; slot++) {
            bool item_found = false;
            for (const auto& priority : get_priority_order()) {
                if (item_found) { break; } // restart the priority iteration
                for (size_t i = 0; i < items.size(); i++) {
                    if (!items[i] || *items[i] != priority) { continue; }

                    crate_.get_inventory()->take_slot(static_cast<int>(i));
                    items_taken[std::format("{}: {}", i + taken_offset,
                                            get_repr(items[i]))] = true;
                    // erase this item from the items array to shift all the other items.
                    items.erase(items.begin() + static_cast<int>(i));
                    taken_offset++;
                    item_found = true;
                    break;
                }
            }
        }
        return items_taken;
    }
}

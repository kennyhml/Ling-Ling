#include "cratestation.h"

#include <unordered_set>

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

    void CrateStation::loot(cv::Mat& loot_img_out, std::vector<LootResult>& contents)
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
            contents = take_high_priority_items();
            std::cout << "[+] " << contents.size() << " items taken out!" << std::endl;
            if (!has_folder) { crate_.get_inventory()->make_new_folder("checked"); }
            crate_.get_inventory()->close();
        }
        else {
            // Take all of the items from the drop, let it vanish so a new one can spawn.
            contents = get_valuable_tooltips();
            do { crate_.get_inventory()->transfer_all(); }
            while (!util::await([this]() -> bool {
                return crate_.get_inventory()->is_open();
            }, std::chrono::seconds(10)));
        }
        // Closing the drop or despawning it often causes a frame-drop
        asa::core::sleep_for(std::chrono::seconds(2));
    }

    std::vector<LootResult> CrateStation::get_valuable_tooltips() const
    {
        int folder_offset = 0;
        const auto items = crate_.get_inventory()->get_current_page_items();
        std::vector<LootResult> res(items.size());

        for (const auto& slot : crate_.get_inventory()->slots) {
            if (slot.is_folder()) { folder_offset++; }
            else { break; }
        }

        for (size_t i = 0; i < items.size(); i++) {
            res[i] = {get_repr(items[i]), false, nullptr};
        }

        // Take the items based on their priority in the priority order.
        std::pmr::unordered_set<size_t> checked(items.size());
        for (int slot_ = 0; slot_ < items.size(); slot_++) {
            bool item_found = false;
            for (const auto& priority : get_priority_order()) {
                if (item_found) { break; }
                for (size_t i = 0; i < items.size(); i++) {
                    if (checked.contains(i) || !items[i] || *items[i] != priority) {
                        continue;
                    }
                    checked.emplace(i);
                    if (inspect_tooltip(items[i]->get_data())) {
                        auto& slot = crate_.get_inventory()->slots[i + folder_offset];
                        crate_.get_inventory()->select_slot(slot, true, true);
                        res[i].tooltip = std::move(slot.get_tooltip());
                    }
                    item_found = true;
                    break;
                }
            }
        }
        return res;
    }

    std::vector<LootResult> CrateStation::take_high_priority_items() const
    {
        int folder_offset = 0;
        auto items = crate_.get_inventory()->get_current_page_items();

        std::vector<LootResult> res(items.size());
        std::vector<int> offsets(res.size());

        for (const auto& slot : crate_.get_inventory()->slots) {
            if (slot.is_folder()) { folder_offset++; }
            else { break; }
        }

        for (size_t i = 0; i < items.size(); i++) {
            res[i] = {get_repr(items[i]), false, nullptr};
        }

        // Take the items based on their priority in the priority order.
        const size_t original_size = items.size();
        for (int slot_ = 0; slot_ < original_size - 1; slot_++) {
            bool item_found = false;
            for (const auto& priority : get_priority_order()) {
                if (item_found) { break; }
                for (size_t i = 0; i < items.size(); i++) {
                    if (!items[i] || *items[i] != priority) { continue; }

                    if (inspect_tooltip(items[i]->get_data())) {
                        auto& slot = crate_.get_inventory()->slots[i + folder_offset];
                        crate_.get_inventory()->select_slot(slot, true, true);
                        res[i + offsets[i]].tooltip = std::move(slot.get_tooltip());
                    }

                    crate_.get_inventory()->take_slot(i + folder_offset);
                    res[i + offsets[i]].looted = true;

                    for (size_t j = i; j < offsets.size() - 1; j++) {
                        offsets[j] = offsets[j + 1] + 1;
                    }

                    items.erase(items.begin() + static_cast<int>(i));
                    item_found = true;
                    break;
                }
            }
        }
        return res;
    }
}

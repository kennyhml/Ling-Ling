#include "lootcratestation.h"
#include "../../common/util.h"
#include "embeds.h"
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>

#include "../../config/config.h"

#define ANY_CRAFTED(name)                                                      \
	Item(name, false, ItemData::ASCENDANT),                                    \
		Item(name, false, ItemData::MASTERCRAFT),                              \
		Item(name, false, ItemData::JOURNEYMAN),                               \
		Item(name, false, ItemData::APPRENTICE),                               \
		Item(name, false, ItemData::RAMSHACKLE),                               \
		Item(name, false, ItemData::PRIMITIVE),

#define ONLY_JM_MC_BPS(name)                                                   \
	Item(name, true, ItemData::JOURNEYMAN),                                    \
		Item(name, true, ItemData::MASTERCRAFT),

#define MC_ASC_CRAFTED_JM_MC_BP(name)                                          \
	Item(name, true, ItemData::MASTERCRAFT),                                   \
		Item(name, true, ItemData::JOURNEYMAN),                                \
		Item(name, false, ItemData::ASCENDANT),                                \
		Item(name, false, ItemData::MASTERCRAFT),

#define ANY_CRAFTED_JM_MC_BP(name)                                             \
	ANY_CRAFTED(name)                                                          \
	ONLY_JM_MC_BPS(name)

namespace llpp::bots::drops
{
    namespace
    {
        using namespace asa::items;

        std::string get_repr(const std::unique_ptr<Item>& item)
        {
            return item ? item->info() : "???";
        }

        std::vector<Item> get_priority_order()
        {
            static std::vector<Item> priority_order{
                // flak / good weapon bps
                ONLY_JM_MC_BPS("Flak Helmet") ONLY_JM_MC_BPS("Flak Chestpiece")
                ONLY_JM_MC_BPS("Flak Leggings") ONLY_JM_MC_BPS("Flak Gauntlets")
                ONLY_JM_MC_BPS("Flak Boots") ONLY_JM_MC_BPS("Fabricated Sniper Rifle")
                ONLY_JM_MC_BPS("Compound Bow")

                // saddles
                MC_ASC_CRAFTED_JM_MC_BP("Giganotosaurus Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Carcharo " "Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Paracer Platform Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Tusoteuthis Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Basi" "losa" "urus" " Sad" "dle")
                MC_ASC_CRAFTED_JM_MC_BP("Rhyniognatha Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Rex Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Stego Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Thylacoleo Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Pteranodon Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Paracer Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Megalodon Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Carbonemys Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Woolly Rhino Saddle")


                // grindables
                ANY_CRAFTED("Electric Prod") ANY_CRAFTED("Fabricated Sniper " "Rifle")
                ANY_CRAFTED("Compound Bow") ANY_CRAFTED("Pump-Action Shotgun")
                ANY_CRAFTED("Assault Rifle") ANY_CRAFTED("Fabricated Pistol")
                ANY_CRAFTED("Riot Shield") ANY_CRAFTED("Riot Helmet")
                ANY_CRAFTED("Riot Chestpiece") ANY_CRAFTED("Riot Leggings")
                ANY_CRAFTED("Riot Gauntlets") ANY_CRAFTED("Riot Boots")
                ANY_CRAFTED("Scuba Leggings") ANY_CRAFTED("Ghilie Mask")
                ANY_CRAFTED("Ghilie Chestpiece") ANY_CRAFTED("Ghilie " "Leggings")
                ANY_CRAFTED("Ghilie Gauntlets") ANY_CRAFTED("Ghilie Boots")
                ANY_CRAFTED("Harpoon " "Launcher") ANY_CRAFTED("Longneck Rifle")
                ANY_CRAFTED("Simple Pistol") ANY_CRAFTED("Metal" " Sick" "le")
                ANY_CRAFTED("Lance") ANY_CRAFTED("Plesiosaur Saddle") ANY_CRAFTED("Lance")
                ANY_CRAFTED("Plesi" "osaur" " Plat" "form " "Saddl" "e")
                ANY_CRAFTED("Ankylo Saddle") ANY_CRAFTED("Tapejara Saddle")
                ANY_CRAFTED("Kaprosuchus " "Saddle")
                Item("Metal " "Arrow", false, ItemData::NONE),
                Item("Rocket Launcher", false, ItemData::NONE), Item("C4 Detonator"),
                Item("C4 Detonator", true, ItemData::NONE),
                Item("Advanced Sniper Bullet", false, ItemData::NONE),
                Item("Simple Bullet", false, ItemData::NONE),
                Item("Rocket Propelled Grenade", true, ItemData::NONE),
                Item("Rocket Propelled Grenade", false, ItemData::NONE),
                Item("Simple Shotgun Ammo", false, ItemData::NONE),
                Item("Poison Grenade", false, ItemData::NONE),
            };
            return priority_order;
        }

        class CrateAlreadyRerolledError : public std::exception
        {
        public:
            CrateAlreadyRerolledError() = default;

            const char* what() const override
            {
                return "Crate has already been rerolled.\n";
            }
        };
    }

    LootCrateStation::LootCrateStation(const std::string& t_name,
                                       CrateManagerConfig& t_config,
                                       asa::structures::CaveLootCrate t_crate,
                                       const bool t_is_first,
                                       const bool t_is_first_in_group) :
        BedStation(t_name, std::chrono::minutes(t_config.interval)),
        TeleportStation(t_name, std::chrono::minutes(t_config.interval)),
        config_(t_config), is_first_(t_is_first), is_first_in_group_(t_is_first_in_group),
        crate_(std::move(t_crate)), vault_(BedStation::name_ + "::VAULT", 350) {}

    core::StationResult LootCrateStation::complete()
    {
        if (config_.uses_teleporters ? !TeleportStation::begin() : !BedStation::begin()) {
            return {
                static_cast<BedStation*>(this), false,
                BedStation::get_time_taken<std::chrono::seconds>(), {}
            };
        }
        BedStation::last_started_ = std::chrono::system_clock::now();

        if (!config_.uses_teleporters) {
            asa::entities::local_player->set_pitch(30);
        } else {
            asa::entities::local_player->stand_up();
            asa::entities::local_player->crouch();
        }

        if (!is_default_) {
            asa::entities::local_player->turn_up(60, std::chrono::milliseconds(500));
        }

        if (is_first_) {
            asa::core::sleep_for(std::chrono::seconds(config_.render_initial_for));
        }

        if (!util::await(
            [this]() -> bool { return asa::entities::local_player->can_access(crate_); },
            std::chrono::seconds(is_first_in_group_ ? config_.render_group_for : 1))) {
            is_crate_up_ = false;
            BedStation::set_completed();
            return {static_cast<BedStation*>(this), false, std::chrono::seconds(0), {}};
        }

        if (!is_crate_up_) {
            is_crate_up_ = true;
            last_discovered_up_ = std::chrono::system_clock::now();
        }

        got_rerolled_ = false;
        const auto quality = crate_.get_crate_quality();

        cv::Mat loot_screenshot;
        std::map<std::string, bool> cherry_picked_items;
        loot_crate(loot_screenshot, cherry_picked_items);

        if (!config_.uses_teleporters) {
            asa::entities::local_player->turn_right();
            asa::entities::local_player->access(vault_);
            asa::entities::local_player->get_inventory()->transfer_all();
            asa::core::sleep_for(std::chrono::seconds(1));
            vault_slots_ = vault_.get_current_slots();
            vault_.get_inventory()->close();
            asa::core::sleep_for(std::chrono::seconds(1));
        }
        BedStation::set_completed();
        const auto elapsed = BedStation::get_time_taken<std::chrono::seconds>();
        core::StationResult result(static_cast<BedStation*>(this), true, elapsed, {});

        if (!should_reroll()) {
            send_success_embed(result, loot_screenshot, quality, ++times_looted_,
                               got_rerolled_);
        }
        else {
            request_reroll(result, loot_screenshot, quality,
                           last_discovered_up_ + max_buff_wait_time_,
                           cherry_picked_items);
        }
        return result;
    }

    void LootCrateStation::loot_crate(cv::Mat& screenshot_out,
                                      std::map<std::string, bool>& cherry_picked_out)
    {
        asa::entities::local_player->access(crate_);
        crate_.get_inventory()->switch_to(asa::interfaces::CraftingInventory::INVENTORY);
        screenshot_out = asa::window::screenshot({1193, 227, 574, 200});
        std::cout << "\t[-] Loot screenshot has been taken.\n";

        if (should_reroll()) {
            try { cherry_picked_out = cherry_pick_items(); }
            catch (const CrateAlreadyRerolledError&) {
                got_rerolled_ = true;
                return loot_crate(screenshot_out, cherry_picked_out);
            }
            if (!crate_.get_inventory()->slots[0].is_folder()) {
                crate_.get_inventory()->make_new_folder("checked");
            }
            asa::core::sleep_for(std::chrono::milliseconds(500));
            crate_.get_inventory()->close();
        }
        else {
            do { crate_.get_inventory()->transfer_all(); }
            while (!util::await([this]() { return !crate_.get_inventory()->is_open(); },
                                std::chrono::seconds(6)));
        }

        asa::core::sleep_for(std::chrono::seconds(2));
        std::cout << "\t[-] The crate has been looted successfully.\n";
    }

    std::map<std::string, bool> LootCrateStation::cherry_pick_items()
    {
        std::cout << "[+] Cherry picking high priority items due to reroll mode...\n";
        std::map<std::string, bool> items_taken{};
        auto items = crate_.get_inventory()->get_current_page_items();
        std::vector<std::string> found_items;

        // set all looted states to 0 initially.
        for (size_t i = 0; i < items.size(); i++) {
            std::string repr = get_repr(items[i]);
            found_items.push_back(repr);
            std::cout << std::format("\t[-] {}: {}\n", i, repr);
            items_taken[std::format("{}: {}", i, repr)] = false;
        }

        if (!last_seen_items_.empty() && (found_items != last_seen_items_)) {
            // if the folder is still here, that means someone rerolled the drop
            // and left the items in it for us to take them and drop them off.
            if (crate_.get_inventory()->slots[0].is_folder()) {
                throw CrateAlreadyRerolledError();
            }
            last_discovered_up_ = std::chrono::system_clock::now();
            std::cout << "last discovered up reset.\n";
        }
        last_seen_items_.clear();
        for (int slot = 0; slot < items.size() && items.size() > 1; slot++) {
            bool item_found = false;
            for (const auto& priority : get_priority_order()) {
                if (item_found) { break; } // restart the priority iteration
                for (size_t i = 0; i < items.size(); i++) {
                    if (!items[i] || *items[i] != priority) { continue; }

                    crate_.get_inventory()->take_slot(static_cast<int>(i));
                    items_taken[std::format("{}: {}", i, get_repr(items[i]))] = true;
                    std::cout << "\t[-] Took " << items[i]->info() << "\n";
                    items.erase(items.begin() + static_cast<int>(i));
                    item_found = true;
                    break;
                }
            }
        }

        for (int i = 0; i < items.size(); i++) {
            last_seen_items_.push_back(get_repr(items[i]));
        }
        return items_taken;
    }

    bool LootCrateStation::has_buff_wait_expired() const
    {
        return util::timedout(last_discovered_up_, max_buff_wait_time_);
    }

    bool LootCrateStation::should_reroll() const
    {
        return !has_buff_wait_expired() && !config_.overrule_reroll_mode &&
            config::bots::drops::reroll_mode.get() && !got_rerolled_;
    }
}

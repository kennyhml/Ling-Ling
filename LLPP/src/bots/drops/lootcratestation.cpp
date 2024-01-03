#include "lootcratestation.h"
#include "../../common/util.h"
#include "embeds.h"
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>

#include "../../config/config.h"

#define ANY_CRAFTED(name) Item(name, false, ItemData::ASCENDANT), \
Item(name, false, ItemData::MASTERCRAFT), \
Item(name, false, ItemData::JOURNEYMAN), \
Item(name, false, ItemData::APPRENTICE), \
Item(name, false, ItemData::RAMSHACKLE), \
Item(name, false, ItemData::PRIMITIVE),

#define ONLY_JM_MC_BPS(name) Item(name, true, ItemData::JOURNEYMAN), \
Item(name, true, ItemData::MASTERCRAFT),

#define MC_ASC_CRAFTED_JM_MC_BP(name) Item(name, true, ItemData::MASTERCRAFT), \
Item(name, true, ItemData::JOURNEYMAN), \
Item(name, false, ItemData::ASCENDANT), \
Item(name, false, ItemData::MASTERCRAFT),

#define ANY_CRAFTED_JM_MC_BP(name) ANY_CRAFTED(name) \
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
                MC_ASC_CRAFTED_JM_MC_BP("Carcharo Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Paracer Platform Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Tusoteuthis Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Basilosaurus Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Rhyniognatha Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Rex Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Stego Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Thylacoleo Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Pteranodon Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Paracer Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Megalodon Saddle")
                MC_ASC_CRAFTED_JM_MC_BP("Carbonemys Saddle")

                // grindables
                ANY_CRAFTED("Electric Prod") ANY_CRAFTED("Fabricated Sniper Rifle")
                ANY_CRAFTED("Compound Bow") ANY_CRAFTED("Pump-Action Shotgun")
                ANY_CRAFTED("Assault Rifle") ANY_CRAFTED("Fabricated Pistol")
                ANY_CRAFTED("Riot Shield") ANY_CRAFTED("Riot Helmet")
                ANY_CRAFTED("Riot Chestpiece") ANY_CRAFTED("Riot Leggings")
                ANY_CRAFTED("Riot Gauntlets") ANY_CRAFTED("Riot Boots")
                ANY_CRAFTED("Scuba Leggings") ANY_CRAFTED("Ghilie Mask")
                ANY_CRAFTED("Ghilie Chestpiece") ANY_CRAFTED("Ghilie Leggings")
                ANY_CRAFTED("Ghilie Gauntlets") ANY_CRAFTED("Ghilie Boots")
                ANY_CRAFTED("Harpoon Launcher") ANY_CRAFTED("Longneck Rifle")
                ANY_CRAFTED("Simple Pistol") ANY_CRAFTED("Metal Sickle")
                ANY_CRAFTED("Lance") ANY_CRAFTED("Plesiosaur Saddle") ANY_CRAFTED("Lance")
                ANY_CRAFTED("Plesiosaur Platform Saddle") ANY_CRAFTED("Ankylo Saddle")
                ANY_CRAFTED("Tapejara Saddle") ANY_CRAFTED("Kaprosuchus Saddle")
                Item("Metal Arrow", false, ItemData::NONE), Item("C4 Detonator"),
                Item("Advanced Sniper Bullet", false, ItemData::NONE),
                Item("Simple Bullet", false, ItemData::NONE),
                Item("Rocket Propelled Grenade", true, ItemData::NONE),
                Item("Rocket Propelled Grenade", false, ItemData::NONE),
                Item("Simple Shotgun Ammo", false, ItemData::NONE),
                Item("Poison Grenade", false, ItemData::NONE),
            };
            return priority_order;
        }
    }

    LootCrateStation::LootCrateStation(const std::string& t_name,
                                       CrateManagerConfig& t_config,
                                       asa::structures::CaveLootCrate t_crate) :
        BaseStation(t_name, std::chrono::minutes(t_config.interval)), config_(t_config),
        crate_(std::move(t_crate)), teleporter_(asa::structures::Teleporter(name_)),
        bed_(asa::structures::SimpleBed(name_)),
        vault_(asa::structures::Container(name_ + "::VAULT", 350)) {};

    core::StationResult LootCrateStation::complete()
    {
        const auto start = std::chrono::system_clock::now();
        go_to();

        if (!util::await([this]() -> bool {
            return asa::entities::local_player->can_access(crate_);
        }, std::chrono::seconds(is_rendered_ ? 0 : config_.render_for))) {
            is_crate_up_ = false;
            set_completed();
            return {this, false, std::chrono::seconds(0), {}};
        }

        if (!is_crate_up_) {
            is_crate_up_ = true;
            last_discovered_up_ = std::chrono::system_clock::now();
        }

        const auto quality = crate_.get_crate_quality();

        cv::Mat loot_screenshot;
        std::map<std::string, bool> cherry_picked_items;
        loot_crate(loot_screenshot, cherry_picked_items);

        if (!config_.uses_teleporters) {
            asa::entities::local_player->turn_right();
            asa::entities::local_player->access(vault_);
            asa::entities::local_player->get_inventory()->transfer_all();
            vault_.inventory->close();
        }
        set_completed();
        const auto elapsed = util::get_elapsed<std::chrono::seconds>(start);
        core::StationResult result(this, true, elapsed, {});

        if (!should_reroll()) {
            send_success_embed(result, loot_screenshot, quality, ++times_looted_);
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
        screenshot_out = asa::window::screenshot({1193, 227, 574, 200});
        std::cout << "\t[-] Loot screenshot has been taken.\n";

        if (should_reroll()) {
            cherry_picked_out = cherry_pick_items();
            crate_.inventory->close();
        }
        else {
            do { crate_.inventory->transfer_all(); }
            while (!util::await([this]() { return !crate_.inventory->is_open(); },
                                std::chrono::seconds(6)));
        }

        asa::core::sleep_for(std::chrono::seconds(2));
        std::cout << "\t[-] The crate has been looted successfully.\n";
    }

    std::map<std::string, bool> LootCrateStation::cherry_pick_items() const
    {
        std::cout << "[+] Cherry picking high priority items due to reroll mode...\n";
        std::map<std::string, bool> items_taken{};
        auto items = crate_.inventory->get_current_page_items();
        // set all looted states to 0 initially.
        for (size_t i = 0; i < items.size(); i++) {
            std::string repr = get_repr(items[i]);
            std::cout << std::format("\t[-] {}: {}\n", i, repr);
            items_taken[std::format("{}: {}", i, repr)] = false;
        }

        for (int slot = 0; slot < items.size() && items.size() > 1; slot++) {
            bool item_found = false;
            for (const auto& priority : get_priority_order()) {
                if (item_found) { break; } // restart the priority iteration
                for (size_t i = 0; i < items.size(); i++) {
                    if (!items[i] || *items[i] != priority) { continue; }

                    crate_.inventory->take_slot(static_cast<int>(i));
                    items_taken[std::format("{}: {}", i, get_repr(items[i]))] = true;
                    std::cout << "\t[-] Took " << items[i]->info() << "\n";
                    items.erase(items.begin() + static_cast<int>(i));
                    item_found = true;
                    break;
                }
            }
        }
        return items_taken;
    }

    void LootCrateStation::go_to()
    {
        if (!config_.uses_teleporters) {
            asa::entities::local_player->fast_travel_to(bed_);
            asa::entities::local_player->turn_down(30);
            return;
        }

        util::await([]() {
            return asa::entities::local_player->can_use_default_teleport();
        }, std::chrono::seconds(10));
        asa::entities::local_player->teleport_to(teleporter_, is_default_dst_);
        if (!is_default_dst_) {
            asa::entities::local_player->turn_up(60, std::chrono::milliseconds(500));
        }
    }

    bool LootCrateStation::has_buff_wait_expired() const
    {
        return util::timedout(last_discovered_up_, max_buff_wait_time_);
    }

    bool LootCrateStation::should_reroll() const
    {
        return !has_buff_wait_expired() && !config_.overrule_reroll_mode &&
            config::bots::drops::reroll_mode.get();
    }
}

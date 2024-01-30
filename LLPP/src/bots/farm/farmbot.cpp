#include "farmbot.h"
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/hud.h>
#include <asapp/interfaces/spawnmap.h>
#include <asapp/items/items.h>

namespace llpp::bots::farm
{
    namespace
    {
        bool should_drop_items(const std::chrono::system_clock::time_point last_drop)
        {
            return asa::interfaces::hud->is_mount_capped() || (
                    util::timedout(last_drop, std::chrono::seconds(15)) && !
                            asa::interfaces::HUD::item_added({37, 521, 85, 90}));
        }
    }

    bool run_while_requested()
    {
        if (!FarmBot::instance()) { return false; }

        FarmBot::instance()->run();
        return true;
    }

    FarmBot::FarmBot(const FarmType t_type, const std::string& t_where,
                     const dpp::user& t_with_who) : requester_(t_with_who),
                                                    resource_(t_type), spawn_at_(t_where),
                                                    mount_("FARM MOUNT") { inst_ = this; }


    void FarmBot::run()
    {
        if (stop_requested_) { return; }
        has_started_ = true;

        const auto start = std::chrono::system_clock::now();
        asa::entities::local_player->fast_travel_to(spawn_at_);


        const auto item = resource_ == METAL
                          ? asa::items::resources::metal
                          : asa::items::resources::wood;


        asa::entities::local_player->set_pitch(90);
        auto last_dropped = std::chrono::system_clock::now();
        auto last_rm_check = std::chrono::system_clock::now();
        auto last_health_check = std::chrono::system_clock::now();

        while (!stop_requested_) {
            mount_.primary_attack();

            if (should_drop_items(last_dropped) && !stop_requested_) {
                drop_trash();
                last_dropped = std::chrono::system_clock::now();
            }

            if (util::timedout(last_rm_check, std::chrono::seconds(1))) {
                last_rm_check = std::chrono::system_clock::now();
                if (asa::entities::local_player->deposited_item(*item)) {
                    help_popcorn(*item);
                }
            }

            if (util::timedout(last_health_check, std::chrono::seconds(10))) {
                if (asa::entities::local_player->is_out_of_food()) {
                    asa::window::press(asa::settings::use_item1);
                }

                if (asa::entities::local_player->is_out_of_water()) {
                    asa::window::press(asa::settings::use_item2);
                }

                if (asa::entities::local_player->is_broken_bones()) {
                    asa::window::press(asa::settings::use_item3);
                }
                last_health_check = std::chrono::system_clock::now();
            }
        }

        destroy();
        asa::entities::local_player->suicide();
        asa::interfaces::spawn_map->spawn_at("RESET BED");
    }

    void FarmBot::drop_trash()
    {
        mount_.open_inventory();

        switch (resource_) {
            case METAL: {
                mount_.get_inventory()->drop_all("o"); // stone, obsidian, wood
                mount_.get_inventory()->drop_all("ry"); // crystal, berry
                mount_.get_inventory()->drop_all("h"); // thatch
                break;
            }
            case WOOD: {
                mount_.get_inventory()->drop_all("h"); // Thatch
                mount_.get_inventory()->drop_all("e"); // berry
                break;
            }
        }
        mount_.get_inventory()->close();
    }

    void FarmBot::help_popcorn(asa::items::Item& item)
    {
        mount_.open_inventory();
        asa::core::sleep_for(std::chrono::milliseconds(500));
        mount_.get_inventory()->search_bar.search_for(item.get_name());
        asa::core::sleep_for(std::chrono::milliseconds(500));

        const auto pos = mount_.get_inventory()->slots[0].area.get_random_location(4);
        click_at(pos, asa::controls::LEFT);

        mount_.get_inventory()->popcorn_all();
        mount_.get_inventory()->close();
    }

    void FarmBot::destroy()
    {
        delete this;
        inst_ = nullptr;
    }

    void FarmBot::mount_farm_tame()
    {
        asa::entities::local_player->access(mount_);
        mount_.get_inventory()->transfer_all();

        while (!mount_.get_inventory()->slots[0].is_empty()) {
            std::cout << "Waiting for items to transfer.." << std::endl;
        }

        asa::entities::local_player->get_inventory()->select_info_tab();

        for (int i = 5; i > 0; i--) {
            auto& slot = asa::entities::local_player->get_inventory()->slots[i];
            if (slot.is_empty()) { continue; }

            auto item = slot.get_item();
            std::cout << i << ": " << (item ? item->get_name() : "?") << std::endl;
            if (item && item->get_name().find("Fur") != std::string::npos) {
                asa::entities::local_player->get_inventory()->select_slot(i);

                do {
                    asa::window::press(asa::settings::use);
                } while (!util::await([&slot]() -> bool { return !slot.is_hovered(); },
                                      std::chrono::seconds(5)));
            }
        }
        asa::entities::local_player->get_inventory()->close();
        asa::entities::local_player->mount(mount_);
    }
}
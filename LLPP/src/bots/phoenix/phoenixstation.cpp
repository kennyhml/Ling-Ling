#include <iostream>
#include <asapp/items/items.h>
#include "phoenixstation.h"
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include "embeds.h"
#include <asapp/interfaces/tribemanager.h>
#include "../../discord/tribelogs/handler.h"

namespace llpp::bots::phoenix
{
    namespace
    {
        void deposit()
        {
            asa::core::sleep_for(300ms);
            asa::controls::press(asa::settings::use);
            asa::core::sleep_for(300ms);
        }
    }

    PhoenixStation::PhoenixStation(std::string t_name,
                             const std::chrono::minutes t_interval) :
            BedStation(std::move(t_name), t_interval),
            vault_(name_ + "::WHIPS", 350),
            dedis_({
                           asa::structures::DedicatedStorage(t_name + "::DEDI01"),
                           asa::structures::DedicatedStorage(t_name + "::DEDI02"),
                           asa::structures::DedicatedStorage(t_name + "::DEDI03"),
                           asa::structures::DedicatedStorage(t_name + "::DEDI04"),
                           asa::structures::DedicatedStorage(t_name + "::DEDI05"),
                           asa::structures::DedicatedStorage(t_name + "::DEDI06")
                   }){ };

    core::StationResult PhoenixStation::complete()
    {
        if (!begin()) {
            return {this, false, get_time_taken<std::chrono::seconds>(), {}};
        }

        // Phoenix Station:
        // 1) Spawn on bed named: PHOENIX::PHOENIX##
        asa::core::sleep_for(std::chrono::seconds(5));

        // Check logs
        asa::interfaces::tribe_manager->open();

        asa::interfaces::tribe_manager->update_tribelogs(discord::handle_tribelog_events);
        asa::core::sleep_for(std::chrono::seconds(1));

        asa::interfaces::tribe_manager->close();

        asa::core::sleep_for(std::chrono::seconds(2));

        // 2) Turn right
        asa::entities::local_player->set_yaw(90);
        asa::core::sleep_for(std::chrono::milliseconds(500));
        // 3) Access the vault
        asa::entities::local_player->access(vault_);
        vault_.get_inventory()->search_bar.search_for("whip");
        asa::core::sleep_for(std::chrono::milliseconds(500));
        // Add this click in case the enter from search_for has it typing in chat
        asa::controls::press(asa::settings::fire);
        asa::core::sleep_for(std::chrono::milliseconds(500));
        if (vault_.get_inventory()->slots[0].is_empty()) {
            throw std::exception("No whips in the vault");
        }
        // 4) Take a whip from the vault
        vault_.get_inventory()->take_slot(0);
        // 5) Equip the whip
        asa::entities::local_player->get_inventory()->select_slot(1);
        // Press "E" on the whip until it equips and the inventory automatically closes,
        // if the whip happens to be broken it will be repaired first granted dedis nearby
        // have the resources.
        while (vault_.get_inventory()->is_open()) {
            asa::controls::press(asa::settings::use);
            asa::core::sleep_for(1s);
        }
        asa::core::sleep_for(1s);
        // 7) Teleport to PHOENIX01
        asa::controls::press(asa::settings::reload);
        asa::core::sleep_for(2s);
        // 8) Look up
        // 9) Whip
        asa::controls::press(asa::settings::fire);
        asa::entities::local_player->set_yaw(-45);
        asa::core::sleep_for(1s);
        asa::controls::press(asa::settings::fire);
        asa::entities::local_player->set_yaw(-135);
        asa::core::sleep_for(1s);
        asa::controls::press(asa::settings::fire);
        asa::entities::local_player->set_yaw(135);
        asa::core::sleep_for(1s);
        asa::controls::press(asa::settings::fire);
        asa::entities::local_player->set_yaw(45);
        asa::core::sleep_for(1s);
        asa::entities::local_player->set_yaw(90);
        asa::core::sleep_for(1s);
        // 12) Teleport to PHOENIX::DEDIS
        asa::core::sleep_for(1s);
        asa::controls::press(asa::settings::reload);
        asa::core::sleep_for(2s);
        // 13) Access the vault
        asa::entities::local_player->access(vault_);
        // 14) Return the whip to the vault
        asa::entities::local_player->get_inventory()->transfer_all("whip");
        // 15) Close the vault
        while (vault_.get_inventory()->is_open()) {
            vault_.get_inventory()->close();
            asa::core::sleep_for(1s);
        }
        // 16) Turn 90 degrees left
        asa::entities::local_player->set_yaw(0);
        asa::core::sleep_for(std::chrono::milliseconds(500));
        // 17) Deposit pearls in dedis
        const int32_t original_yaw = asa::entities::local_player->get_yaw();
        asa::entities::local_player->set_pitch(0);
        asa::entities::local_player->turn_left(20);
        deposit();
        asa::entities::local_player->turn_right(40);
        deposit();

        asa::entities::local_player->set_pitch(-20);
        asa::core::sleep_for(std::chrono::milliseconds(500));
        deposit();
        asa::entities::local_player->turn_left(40);
        deposit();

        asa::entities::local_player->crouch();
        asa::entities::local_player->set_pitch(30);
        deposit();
        asa::entities::local_player->turn_right(40);
        deposit();

        asa::entities::local_player->set_yaw(original_yaw);
        asa::entities::local_player->set_pitch(0);

        asa::entities::local_player->get_inventory()->open();

        if (!util::await([] {
            return asa::entities::local_player->get_inventory()->slots[5].is_empty();
        }, 10s)) {

        }
        asa::entities::local_player->get_inventory()->close();
        asa::core::sleep_for(1s);

        const auto time_taken = get_time_taken<std::chrono::seconds>();
        const core::StationResult res(this, true, time_taken, {});
        set_completed();
        send_pearls_collected(res);

        return res;
    }

}

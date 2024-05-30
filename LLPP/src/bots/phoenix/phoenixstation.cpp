#include <iostream>
#include <asapp/items/items.h>
#include "phoenixstation.h"
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include "embeds.h"

namespace llpp::bots::phoenix
{
    namespace
    {
        void get_pearls()
        {

        }

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
                   }){ get_pearls(); };

    core::StationResult PhoenixStation::complete()
    {
        if (!begin()) {
            return {this, false, get_time_taken<std::chrono::seconds>(), {}};
        }

        // Phoenix Station:
        // 1) Spawn on bed named: PHOENIX::DEDIS
        // 2) Turn right
        asa::entities::local_player->turn_right();
        asa::core::sleep_for(std::chrono::milliseconds(500));
        // 3) Access the vault
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
        // 8) Look up
        // 9) Whip
        // 10) Turn 90 degrees left
        // 11) Repeat 8->9 until looking original direction
        // 12) Teleport to PHOENIX::DEDIS
        // 13) Access the vault
        // 14) Return the whip to the vault
        // 15) Close the vault
        // 16) Turn 90 degrees left
        asa::entities::local_player->turn_left();
        asa::core::sleep_for(std::chrono::milliseconds(500));
        // 17) Deposit pearls in dedis
        deposit_pearls();

        const auto time_taken = get_time_taken<std::chrono::seconds>();
        const core::StationResult res(this, true, time_taken, {});
        set_completed();
        send_pearls_collected(res);

        return res;
    }

    core::StationResult PhoenixStation::deposit_pearls()
    {
        if (!begin(true)) {
            return {this, false, get_time_taken<>(), {}};
        }

        const int32_t original_yaw = asa::entities::local_player->get_yaw();
        asa::entities::local_player->set_pitch(0);
        asa::entities::local_player->crouch();

        asa::entities::local_player->turn_down(40);
        asa::entities::local_player->turn_left(20);
        deposit();
        asa::entities::local_player->turn_right(40);
        deposit();

        asa::entities::local_player->stand_up();
        deposit();
        asa::entities::local_player->turn_left(40);
        deposit();

        asa::entities::local_player->turn_up(60);
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
        return {this, true, get_time_taken<>(), {}};
    }

}

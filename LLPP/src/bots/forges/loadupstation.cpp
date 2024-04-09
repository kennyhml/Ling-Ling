#include "loadupstation.h"
#include <asapp/core/state.h>
#include <asapp/interfaces/hud.h>

namespace llpp::bots::forges
{
    namespace
    {
        bool capped_from(const asa::structures::DedicatedStorage& dedi)
        {
            asa::core::sleep_for(300ms);
            asa::entities::local_player->access(dedi);
            dedi.get_inventory()->transfer_all();
            dedi.get_inventory()->close();
            asa::core::sleep_for(700ms);
            return asa::interfaces::hud->is_player_capped();
        }
    }

    LoadupStation::LoadupStation(const std::string& t_name, std::string t_material)
        : TeleportStation(t_name, 0min), material_(std::move(t_material)),
          dedis_({
              asa::structures::DedicatedStorage(t_name + "::DEDI01"),
              asa::structures::DedicatedStorage(t_name + "::DEDI02"),
              asa::structures::DedicatedStorage(t_name + "::DEDI03"),
              asa::structures::DedicatedStorage(t_name + "::DEDI04"),
              asa::structures::DedicatedStorage(t_name + "::DEDI05"),
              asa::structures::DedicatedStorage(t_name + "::DEDI06")
          }) {}

    core::StationResult LoadupStation::complete()
    {
        if (!begin(true)) {
            return {this, false, get_time_taken<>(), {}};
        }
        // We are currently looking all the way down due to teleporting, make sure we reset.
        asa::entities::local_player->set_pitch(0);
        const int32_t original_yaw = asa::entities::local_player->get_yaw();

        if (!get_slotcap()) {
            last_empty_ = std::chrono::system_clock::now();
        }
        asa::entities::local_player->set_yaw(original_yaw);
        asa::entities::local_player->set_pitch(0);

        return {this, true, get_time_taken<>(), {}};
    }

    bool LoadupStation::get_slotcap()
    {
        asa::entities::local_player->crouch();

        asa::entities::local_player->turn_down(40);
        asa::entities::local_player->turn_left(20);
        if (capped_from(dedis_[0])) { return true; }

        asa::entities::local_player->turn_right(40);
        if (capped_from(dedis_[1])) { return true; }

        asa::entities::local_player->stand_up();
        if (capped_from(dedis_[2])) { return true; }

        asa::entities::local_player->turn_left(40);
        if (capped_from(dedis_[3])) { return true; }

        asa::entities::local_player->turn_up(60);
        if (capped_from(dedis_[4])) { return true; }

        asa::entities::local_player->turn_right(40);
        if (capped_from(dedis_[5])) { return true; }

        // We were unable to get a slotcap, put whatever we took back.
        asa::controls::press(asa::settings::use);
        asa::core::sleep_for(500ms);
        return false;
    }
}

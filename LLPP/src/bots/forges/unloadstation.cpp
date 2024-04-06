#include "unloadstation.h"
#include <asapp/core/state.h>


namespace llpp::bots::forges
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

    UnloadStation::UnloadStation(const std::string& t_name, std::string t_material)
        : TeleportStation(t_name, 0min), material_(std::move(t_material)),
          dedis_({
              asa::structures::DedicatedStorage(t_name + "::DEDI01"),
              asa::structures::DedicatedStorage(t_name + "::DEDI02"),
              asa::structures::DedicatedStorage(t_name + "::DEDI03"),
              asa::structures::DedicatedStorage(t_name + "::DEDI04"),
              asa::structures::DedicatedStorage(t_name + "::DEDI05"),
              asa::structures::DedicatedStorage(t_name + "::DEDI06")
          }) {}

    core::StationResult UnloadStation::complete()
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
        return {this, true, get_time_taken<>(), {}};
    }
}

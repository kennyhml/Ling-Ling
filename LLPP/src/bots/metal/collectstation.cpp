#include "collectstation.h"

#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>

namespace llpp::bots::metal
{
    CollectStation::CollectStation(const std::string& t_name) : BedStation(
        t_name, std::chrono::minutes(5)) {}

    core::StationResult CollectStation::complete()
    {
        asa::entities::local_player->suicide();

        if (!begin(false)) {
            set_completed();
            return {this, false, get_time_taken<>(), {}};
        }

        asa::entities::local_player->crouch();
        asa::entities::local_player->set_pitch(90);
        asa::core::sleep_for(500ms);

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 12; j++) {
                asa::entities::local_player->pick_up_all();
                asa::entities::local_player->turn_right(360 / 12);
                asa::entities::local_player->pick_up_all();
            }
            asa::entities::local_player->turn_up(90 / 8);
        }

        asa::entities::local_player->set_pitch(30);
        asa::entities::local_player->set_yaw(0);
        asa::core::sleep_for(1s);

        asa::controls::press(asa::settings::use);
        asa::entities::local_player->set_pitch(0);
        asa::entities::local_player->stand_up();
        asa::core::sleep_for(1s);
        asa::controls::press(asa::settings::use);

        asa::entities::local_player->suicide();
        return {this, true, get_time_taken<>(), {}};
    }
}

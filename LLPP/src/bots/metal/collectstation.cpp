#include "collectstation.h"

#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/hud.h>

namespace llpp::bots::metal
{
    CollectStation::CollectStation(const std::string& t_name) : BedStation(
        t_name, std::chrono::minutes(5)) {}

    core::StationResult CollectStation::complete()
    {
        static asa::window::Rect roi(21, 445, 113, 301);

        asa::entities::local_player->suicide();

        if (!begin(false)) {
            set_completed();
            return {this, false, get_time_taken<>(), {}};
        }
        asa::core::sleep_for(10s);

        asa::entities::local_player->crouch();
        asa::entities::local_player->set_pitch(90);
        asa::core::sleep_for(500ms);

        asa::entities::local_player->pick_up_all();

        if (!util::await([]() { return asa::interfaces::HUD::item_added(roi); }, 5s)) {
            for (int i = 0; i < 6; i++) {
                for (int j = 0; j < 12; j++) {
                    asa::entities::local_player->pick_up_all();
                    asa::entities::local_player->turn_right(360 / 12);
                    asa::entities::local_player->pick_up_all();
                }
                asa::entities::local_player->turn_up(45 / 8);
            }
        }

        asa::entities::local_player->set_pitch(30);
        asa::entities::local_player->set_yaw(0);
        asa::core::sleep_for(1s);
        asa::controls::press(asa::settings::use);
        asa::core::sleep_for(1s);

        asa::entities::local_player->stand_up();
        asa::entities::local_player->set_pitch(10);
        asa::core::sleep_for(1s);
        asa::controls::press(asa::settings::use);
        asa::core::sleep_for(1s);

        asa::entities::local_player->set_pitch(-20);
        asa::core::sleep_for(1s);
        asa::controls::press(asa::settings::use);
        asa::core::sleep_for(1s);

        asa::entities::local_player->suicide();
        return {this, true, get_time_taken<>(), {}};
    }
}

#include "woodstation.h"

#include <asapp/core/state.h>
#include <asapp/interfaces/hud.h>

namespace llpp::bots::farm
{
    namespace
    {
        bool can_keep_harvesting()
        {
            return asa::interfaces::hud->can_harvest_target()
                   && !asa::interfaces::hud->is_player_capped();
        }
    }

    WoodStation::WoodStation(const std::string& t_name)
        : TeleportStation(t_name, 0min) {}

    core::StationResult WoodStation::complete()
    {
        set_default_destination(false);
        if (!begin(false)) {
            return {this, false, get_time_taken<>(), {}};
        }
        asa::core::sleep_for(1s);
        asa::entities::local_player->set_pitch(0);
        asa::core::sleep_for(1s);

        while (can_keep_harvesting()) {
            asa::controls::mouse_down(asa::controls::LEFT);
        }
        asa::controls::mouse_up(asa::controls::LEFT);

        return {this, true, get_time_taken<>(), {}};
    }
}

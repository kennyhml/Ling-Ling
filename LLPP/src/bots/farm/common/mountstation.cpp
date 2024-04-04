#include "mountstation.h"
#include <asapp/entities/localplayer.h>

namespace llpp::bots::farm
{
    MountStation::MountStation(const std::string& t_name,
                               std::shared_ptr<asa::entities::DinoEntity> t_anky,
                               std::unique_ptr<RenderStation> t_render_station)
        : BedStation(t_name, 0min), anky_(std::move(t_anky)),
          render_station_(std::move(t_render_station)) {}

    core::StationResult MountStation::complete()
    {
        if (render_station_) {
            if (!render_station_->complete().success) {
                return {this, false, get_time_taken<>(), {}};
            }
        }

        if (!begin(true)) {
            return {this, false, get_time_taken<>(), {}};
        }

        util::await([this] {
            return asa::entities::local_player->can_ride(*anky_);
        }, 30s);

        asa::entities::local_player->mount(*anky_);
        asa::entities::local_player->set_pitch(90);
        asa::entities::local_player->turn_left();
        return {this, true, get_time_taken<>(), {}};
    }
}

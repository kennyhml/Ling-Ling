#include "suicidestation.h"
#include "../../common/util.h"
#include "../suicide/suicideembed.h"
#include <asapp/entities/localplayer.h>

namespace llpp::bots
{
    SuicideStation::SuicideStation(std::string t_bed_name, std::string t_respawn_bed) :
        BedStation(std::move(t_bed_name), std::chrono::minutes(5)),
        respawn_station_(std::move(t_respawn_bed), std::chrono::minutes(5)) {};

    core::StationResult SuicideStation::complete()
    {
        if (!begin()) {
            return {this, false, get_time_taken<std::chrono::seconds>(), {}};
        }

        asa::entities::local_player->suicide();
        set_completed();

        return {this, true, get_time_taken<std::chrono::seconds>(), {}};
    }
}

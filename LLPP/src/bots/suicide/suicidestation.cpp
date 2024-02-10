#include "suicidestation.h"
#include "../../common/util.h"
#include "../../core/basestation.h"
#include "suicideembed.h"
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/spawnmap.h>
#include <asapp/structures/simplebed.h>

namespace llpp::bots::suicide
{
    SuicideStation::SuicideStation(std::string t_bed_name, std::string t_respawn_bed) :
        BedStation(t_bed_name, std::chrono::minutes(5)), respawn_bed(t_respawn_bed) {};

    core::StationResult SuicideStation::complete()
    {
        if (!begin()) {
            return {this, false, get_time_taken<std::chrono::seconds>(), {}};
        }

        asa::entities::local_player->suicide();
        asa::interfaces::spawn_map->spawn_at(respawn_bed.get_name());
        std::this_thread::sleep_for(std::chrono::seconds(10));
        set_completed();

        return {this, true, get_time_taken<std::chrono::seconds>(), {}};
    }
}

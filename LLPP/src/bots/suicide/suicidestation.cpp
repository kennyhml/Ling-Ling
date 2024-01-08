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
        BaseStation(t_bed_name, std::chrono::minutes(5)), death_bed(t_bed_name),
        respawn_bed(t_respawn_bed) {};

    core::StationResult SuicideStation::complete()
    {
        auto start = std::chrono::system_clock::now();
        asa::entities::local_player->fast_travel_to(death_bed);
        asa::entities::local_player->suicide();
        asa::interfaces::spawn_map->spawn_at(respawn_bed.name);
        std::this_thread::sleep_for(std::chrono::seconds(10));
        set_completed();

        auto timeTaken = util::get_elapsed<std::chrono::seconds>(start);
        auto data = core::StationResult(this, true, timeTaken, {});
        // send_suicided_embed(data, death_bed.name, respawn_bed.name);
        return data;
    }
}

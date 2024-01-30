#include "sparkpowderstation.h"
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include "embeds.h"
#include "../../common/util.h"

namespace llpp::bots::crafting
{
    core::StationResult SparkpowderStation::complete()
    {
        asa::entities::local_player->fast_travel_to(bed_);
        const auto start = std::chrono::system_clock::now();

        requeue();
        empty();

        set_completed();
        const auto time_taken = util::get_elapsed<std::chrono::seconds>(start);
        const core::StationResult res(this, true, time_taken, {});
        send_spark_crafted(res, was_still_crafting_);
        return res;
    }
}

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

        asa::entities::local_player->access(chem_bench_);
        asa::core::sleep_for(std::chrono::milliseconds(500));
        if (chem_bench_.get_current_slots() > 0) { empty_into_dedis(); }
        requeue_crafts();

        set_completed();
        const auto time_taken = util::get_elapsed<std::chrono::seconds>(start);
        const core::StationResult res(this, true, time_taken, {});
        send_spark_crafted(res);
        return res;
    }
}

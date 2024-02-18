#include "sparkpowderstation.h"
#include <asapp/core/state.h>
#include "embeds.h"
#include "../../common/util.h"

namespace llpp::bots::crafting
{
    core::StationResult SparkpowderStation::complete()
    {
        if (!begin()) {
            return {this, false, get_time_taken<std::chrono::seconds>(), {}};
        }

        requeue();
        empty();

        set_completed();
        const core::StationResult res(this, true, get_time_taken<std::chrono::seconds>(),
                                      {});
        send_spark_crafted(res, was_still_crafting_);
        return res;
    }
}

#include "gunpowderstation.h"
#include "embeds.h"

namespace llpp::bots::crafting
{
    core::StationResult GunpowderStation::complete()
    {
        if (!begin()) {
            return {this, false, get_time_taken<std::chrono::seconds>(), {}};
        }

        requeue();
        empty();

        set_completed();
        const core::StationResult res(this, true, get_time_taken<std::chrono::seconds>(),
                                      {});
        send_gunpowder_crafted(res, was_still_crafting_);
        return res;
    }
}

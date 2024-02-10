#include "gunpowderstation.h"
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/tribemanager.h>
#include "embeds.h"
#include "../../core/discord.h"

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

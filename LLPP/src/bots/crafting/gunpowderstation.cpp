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
        asa::entities::local_player->fast_travel_to(bed_, AccessFlags_Default,
                                                    TravelFlags_NoTravelAnimation);

        asa::interfaces::tribe_manager->update_tribelogs(core::discord::handle_tribelogs);
        asa::core::sleep_for(std::chrono::seconds(1));

        const auto start = std::chrono::system_clock::now();

        requeue();
        empty();

        set_completed();
        const auto time_taken = util::get_elapsed<std::chrono::seconds>(start);
        const core::StationResult res(this, true, time_taken, {});
        send_gunpowder_crafted(res, was_still_crafting_);
        return res;
    }
}

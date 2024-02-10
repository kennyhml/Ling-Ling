#include "renderstation.h"
#include "../../common/util.h"
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/tribemanager.h>
#include "../../core/discord.h"

namespace llpp::bots::render
{
    RenderStation::RenderStation(std::string t_name, std::chrono::seconds t_load_for)
        : BedStation(std::move(t_name), std::chrono::minutes(5)),
          render_duration(t_load_for) {};

    core::StationResult RenderStation::complete()
    {
        if (!begin()) {
            return {this, false, get_time_taken<std::chrono::seconds>(), {}};
        }

        std::this_thread::sleep_for(render_duration);

        asa::entities::local_player->fast_travel_to(gateway_bed, AccessFlags_Default,
                                                    TravelFlags_NoTravelAnimation);

        asa::interfaces::tribe_manager->update_tribelogs(core::discord::handle_tribelogs,
                                                         std::chrono::seconds(0));
        set_completed();
        return {this, true, get_time_taken<std::chrono::seconds>(), {}};
    }
}

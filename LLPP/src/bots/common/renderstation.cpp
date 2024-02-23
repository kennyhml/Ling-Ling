#include "renderstation.h"
#include "../../common/util.h"
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/tribemanager.h>

namespace llpp::bots
{
    RenderStation::RenderStation(std::string t_name,
                                 const std::chrono::seconds t_load_for)
        : BedStation(std::move(t_name), std::chrono::minutes(5)),
          render_duration_(t_load_for) {};

    core::StationResult RenderStation::complete()
    {
        if (!begin()) {
            return {this, false, get_time_taken<std::chrono::seconds>(), {}};
        }

        std::this_thread::sleep_for(render_duration_);
        gateway_.complete();
        set_completed();
        return {this, true, get_time_taken<std::chrono::seconds>(), {}};
    }
}

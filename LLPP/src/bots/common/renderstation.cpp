#include "renderstation.h"

#include <asapp/core/init.h>
#include <asapp/core/state.h>

#include "../../common/util.h"
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/console.h>
#include <asapp/interfaces/tribemanager.h>

namespace llpp::bots
{
    RenderStation::RenderStation(std::string t_name,
                                 const std::chrono::seconds t_load_for,
                                 const bool t_src_reconnect)
        : BedStation(std::move(t_name), std::chrono::minutes(5)),
          render_duration_(t_load_for),
          src_reconnect_(t_src_reconnect) {};

    core::StationResult RenderStation::complete()
    {
        if (!begin()) {
            return {this, false, get_time_taken<std::chrono::seconds>(), {}};
        }

        if (src_reconnect_) {
            asa::core::sleep_for(5s);
            asa::interfaces::console->execute("reconnect");
            util::await([]() -> bool {
                return asa::entities::local_player->is_in_connect_screen();
            }, 1min);
        }

        std::this_thread::sleep_for(render_duration_);
        gateway_.complete();
        set_completed();
        return {this, true, get_time_taken<std::chrono::seconds>(), {}};
    }
}

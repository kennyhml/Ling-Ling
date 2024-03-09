#include "bed_crate_station.h"

#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>

namespace llpp::bots::drops
{


    BedCrateStation::BedCrateStation(const std::string& t_name,
                                     CrateManagerConfig* t_config,
                                     asa::structures::CaveLootCrate t_crate,
                                     const bool t_is_first,
                                     const bool t_is_first_in_group)
        : BedStation(t_name, std::chrono::minutes(t_config->interval)),
          CrateStation(t_config, std::move(t_crate), t_is_first, t_is_first_in_group),
          vault_(name_ + "::DROPOFF", 350) {}


    core::StationResult BedCrateStation::complete()
    {
        if (!begin()) {
            return {this, false, get_time_taken<std::chrono::seconds>(), {}};
        }

        asa::entities::local_player->set_pitch(30);
        is_up_ = await_crate_loaded();
        if (!is_up_) { return {this, false, get_time_taken<std::chrono::seconds>(), {}}; }

        cv::Mat loot_image;
        std::map<std::string, bool> items_taken;

        loot(loot_image, items_taken);
        // only deposit items if we actually got any.
        if (!should_reroll() || any_looted(items_taken)) {
            deposit_items();
        }

        set_completed();
    }


    void BedCrateStation::deposit_items() {}
}

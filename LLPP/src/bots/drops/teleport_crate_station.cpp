#include "teleport_crate_station.h"
#include "embeds.h"
#include "../../discord/bot.h"
#include <asapp/entities/localplayer.h>

namespace llpp::bots::drops
{
    core::StationResult TeleportCrateStation::complete()
    {
        if (!begin()) {
            return {this, false, get_time_taken<std::chrono::seconds>(), {}};
        }

        asa::entities::local_player->set_pitch(30);
        // Sometimes when arriving at the target teleporter it wont be rendered yet
        // causing the player to float and stand up. The best way to prevent this issue
        // is to stand up and recrouch when we are likely to have entered such an area.
        if (is_first_) {
            asa::entities::local_player->stand_up();
            asa::entities::local_player->crouch();
        }

        if (!is_default_) {
            asa::entities::local_player->turn_up(60, std::chrono::milliseconds(500));
        }

        is_up_ = await_crate_loaded();
        if (!is_up_) { return {this, false, get_time_taken<std::chrono::seconds>(), {}}; }

        cv::Mat loot_image;
        std::map<std::string, bool> items_taken;
        const auto quality = crate_.get_crate_quality();

        loot(loot_image, items_taken);
        set_completed();
        const core::StationResult res(this, true, get_time_taken<std::chrono::seconds>(),
                                      {});
        dpp::message msg;
        if (should_reroll()) {
            msg = get_looted_message(res, loot_image, quality, ++times_looted_);
        }
        else {
            msg = get_reroll_message(res, loot_image, quality,
                                     last_found_up_ + buff_max_wait_, items_taken);
        }
        discord::get_bot()->message_create(msg);
        return res;
    }
}

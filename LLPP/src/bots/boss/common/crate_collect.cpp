#include "crate_collect.h"

#include <iostream>
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/hud.h>
#include <asapp/util/util.h>

namespace llpp::bots::boss
{
    namespace
    {
        asa::window::Color get_color_of_tier(const CrateTier tier)
        {
            switch (tier) {
                case GAMMA: return {5, 246, 5};
                default: return {0, 0, 0};;
            }
        }
    }

    bool collect_element_crate(const CrateTier tier)
    {
        std::chrono::system_clock::time_point last_access_attempt;
        const auto start = std::chrono::system_clock::now();
        asa::structures::Container crate("BOSS LOOT CRATE", 0);

        const auto color = get_color_of_tier(tier);

        while (!crate.get_inventory()->is_open()) {
            asa::controls::key_down("w");

            // Check if we still see the timer after 60s, if not we didnt make it in time.
            if (util::timedout(start, 1min) &&
                !asa::interfaces::hud->is_boss_teleport_out_active()) {
                asa::controls::key_up("w");
                return false;
            }

            // Try to access the crate every 5s, if we spammed it we might close it as we
            // just opened it resulting in an error.
            if (util::timedout(last_access_attempt, 5s)) {
                asa::controls::press(asa::settings::access_inventory);
                last_access_attempt = std::chrono::system_clock::now();
            }

            if (!asa::entities::local_player->turn_to_closest_waypoint(color, 20)) {
                // TODO: Handle cases where we cant locate the crate (too far away?)
            }
        }
        asa::controls::key_up("w");
        // Take all from the crate until it disappears, just like a loot crate.
        do {
            crate.get_inventory()->transfer_all();
        } while (!util::await([&crate] {
            return !crate.get_inventory()->is_open();
        }, 5s));
        return true;
    }
}

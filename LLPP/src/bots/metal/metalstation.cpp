#include "metalstation.h"
#include <asapp/core/state.h>
#include <asapp/interfaces/hud.h>

#include "data.h"

namespace llpp::bots::metal
{
    core::StationResult MetalStation::complete()
    {
        is_default_ = true;
        teleport_unsafe_ = true;

        const bool check_logs = util::timedout(last_log_check, std::chrono::seconds(60));

        if (!begin(check_logs)) {
            set_completed();
            return {this, false, get_time_taken<>(), {}};
        }
        if (check_logs) { last_log_check = std::chrono::system_clock::now(); }
        else { asa::core::sleep_for(std::chrono::milliseconds(500)); }

        swing();

        while (!asa::interfaces::hud->can_default_teleport()) {
            anky.go_back(std::chrono::milliseconds(20));
            asa::core::sleep_for(std::chrono::milliseconds(200));
        }
        set_completed();
        return {this, true, get_time_taken<>(), {}};
    }

    void MetalStation::swing()
    {
        // load the amount of times to swing from our config if we havent yet
        if (!swing_times_) { swing_times_ = get_swings(get_name()); }
        // If our config doesnt know how many swings we need either, then it has never
        // been determined. Check it here and remember it for next time.
        // This should only have to be done once per station ever.
        if (!swing_times_) {
            swing_times_ = count_swings();
            set_swings(get_name(), swing_times_);
            return;
        }
        // Make sure we either press W or S on every swing for the default popup.
        for (int i = 0; i < swing_times_; i++) {
            anky.primary_attack();
            asa::core::sleep_for(std::chrono::milliseconds(2800));
            anky.go_back(std::chrono::milliseconds(20));
        }
    }

    int MetalStation::count_swings()
    {
        static asa::window::Rect roi(21, 445, 113, 301);
        int hit_count = 0;

        while (true) {
            anky.primary_attack();

            // Wait to harvest an item, give it up to 10 seconds in case of lag.
            if (!util::await([]() -> bool {
                return asa::interfaces::HUD::item_added(roi);
            }, std::chrono::seconds(10))) { break; }
            anky.go_back(std::chrono::milliseconds(20));

            hit_count++;
            // wait for the received popup to disappear.
            util::await([]() -> bool { return !asa::interfaces::HUD::item_added(roi); },
                        std::chrono::seconds(10));
        }
        return hit_count;
    }
}

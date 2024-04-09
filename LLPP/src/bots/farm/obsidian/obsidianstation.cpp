#include "obsidianstation.h"
#include "../common/data.h"
#include <asapp/core/state.h>
#include <asapp/interfaces/hud.h>

namespace llpp::bots::farm
{
    ObsidianStation::ObsidianStation(std::string t_name,
                                     std::shared_ptr<asa::entities::DinoEntity> t_anky)
        : TeleportStation(std::move(t_name), 0min), anky_(std::move(t_anky)) {}

    core::StationResult ObsidianStation::complete()
    {
        set_default_destination(true);
        // Allows us to instantly gain execution control back after the source tp
        // was left (default popup disappeared). That way we can start swinging ahead
        // of time and save alot of time while making sure we actually reached the tp
        // afterwards to avoid desyncing.
        set_unsafe_load(true);
        const bool check_logs = should_check_logs();
        if (!begin(check_logs)) {
            set_completed();
            return {this, false, get_time_taken<>(), {}};
        }
        if (check_logs) { last_logs_ = std::chrono::system_clock::now(); } else {
            asa::core::sleep_for(500ms);
        }

        harvest_obsidian();

        while (!asa::interfaces::hud->can_default_teleport()) {
            anky_->go_back(200ms);
            util::await([] { return asa::interfaces::hud->can_default_teleport(); }, 5s);
        }
        set_completed();
        return {this, true, get_time_taken<>(), {}};
    }

    void ObsidianStation::harvest_obsidian()
    {
        // load the amount of times to swing from our config if we havent yet
        if (!swing_times_) { swing_times_ = get_swings(get_name()); }
        // If our config doesnt know how many swings we need either, then it has never
        // been determined. Check it here and remember it for next time.
        // This should only have to be done once per station ever.
        if (!swing_times_) {
            swing_times_ = count_required_swings();
            set_swings(get_name(), swing_times_);
            return;
        }
        // Make sure we either press W or S on every swing for the default popup.
        for (int i = 0; i < swing_times_ + 1; i++) {
            anky_->primary_attack();
            asa::core::sleep_for(2200ms);
            anky_->go_back(200ms);

            if (asa::interfaces::hud->is_mount_capped()) { drop_trash(); }
        }
    }

    int ObsidianStation::count_required_swings() const
    {
        static asa::window::Rect roi(21, 445, 113, 301);
        int hit_count = 0;
        const auto start = std::chrono::system_clock::now();

        while (!util::timedout(start, 1min)) {
            anky_->primary_attack();

            // Wait to harvest an item, give it up to 10 seconds in case of lag.
            if (!util::await([]() -> bool {
                return asa::interfaces::HUD::item_added(roi);
            }, 10s)) { break; }

            hit_count++;
            // wait for the received popup to disappear.
            util::await([]() -> bool { return !asa::interfaces::HUD::item_added(roi); },
                        10s);
            if (asa::interfaces::hud->is_mount_capped()) { drop_trash(); }
            anky_->go_back(200ms);
        }
        return hit_count;
    }

    void ObsidianStation::drop_trash() const
    {
        anky_->open_inventory();
        anky_->get_inventory()->drop_all("t"); // metal, crystal, stone, flint
        anky_->get_inventory()->close();
        asa::core::sleep_for(1s);
    }
}

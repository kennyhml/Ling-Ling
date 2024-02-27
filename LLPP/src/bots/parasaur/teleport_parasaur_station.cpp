#include "teleport_parasaur_station.h"
#include "embeds.h"
#include "../../discord/tribelogs/handler.h"
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/hud.h>
#include <asapp/interfaces/tribemanager.h>

namespace llpp::bots::parasaur
{
    namespace
    {
        void find_teleporter()
        {
            constexpr int per_turn = 30;
            asa::entities::local_player->set_pitch(90);
            asa::core::sleep_for(std::chrono::milliseconds(500));

            for (int i = 0; i < 360 / per_turn; i++) {
                if (asa::interfaces::HUD::can_teleport()) { break; }
                asa::entities::local_player->turn_right(30);
            }
        }

        void let_load(int how_many_seconds, const bool check_logs)
        {
            // allows us to keep it open while we wait
            if (check_logs) { asa::interfaces::tribe_manager->open(); }

            do {
                if (check_logs) {
                    asa::interfaces::tribe_manager->update_tribelogs(
                        discord::handle_tribelog_events);
                }
                // wait in at most 10 second intervals
                const int sleep = std::min(how_many_seconds, 10);
                asa::core::sleep_for(std::chrono::seconds(sleep));
                how_many_seconds -= sleep;
            }
            while (how_many_seconds > 0);

            if (check_logs) { asa::interfaces::tribe_manager->close(); }
        }
    }

    TeleportParasaurStation::TeleportParasaurStation(std::string t_real_name,
                                                     const ParasaurConfig& t_config)
        : TeleportStation(t_config.name, std::chrono::minutes(t_config.interval)),
          real_name_(std::move(t_real_name)), check_logs_(t_config.check_logs),
          tag_level_(t_config.alert_level),
          load_(std::chrono::seconds(t_config.load_for)),
          parasaur_(name_ + "::PARASAUR") {}

    core::StationResult TeleportParasaurStation::complete()
    {
        find_teleporter();
        if (!begin()) {
            return {this, false, get_time_taken<std::chrono::seconds>(), {}};
        }

        asa::entities::local_player->stand_up();
        asa::entities::local_player->crouch();

        // let the station load for at least 15s if we detected something at the previous
        // station in order to get rid of the lingering alert.
        int load_for = static_cast<int>(load_.count());
        if (has_lingering_ping()) { load_for = std::max(load_for, 15); }

        let_load(load_for, check_logs_);

        if (asa::interfaces::hud->detected_enemy()) {
            send_enemy_detected(real_name_, tag_level_);
            last_detected_ = std::chrono::system_clock::now();
        }

        set_completed();
        return {this, true, get_time_taken<std::chrono::seconds>(), {}};
    }

    bool TeleportParasaurStation::has_lingering_ping()
    {
        return last_detected_ != std::chrono::system_clock::time_point() && !
            util::timedout(last_detected_, std::chrono::seconds(10));
    }
}

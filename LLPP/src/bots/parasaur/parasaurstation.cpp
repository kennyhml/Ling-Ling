#include "parasaurstation.h"

#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/hud.h>
#include <asapp/interfaces/tribemanager.h>

#include "embeds.h"
#include "../../discord/tribelogs/handler.h"

namespace llpp::bots::parasaur
{
    ParasaurStation::ParasaurStation(const std::string& t_real_name,
                                     const ParasaurConfig& t_config)
        : TeleportStation(t_config.name, std::chrono::minutes(t_config.interval)),
          BedStation(t_config.name, std::chrono::minutes(t_config.interval)),
          real_name_(t_real_name), use_teleporter_(t_config.is_teleporter),
          check_logs_(t_config.check_logs), tag_level_(t_config.alert_level),
          load_(std::chrono::seconds(t_config.load_for)),
          parasaur_(BedStation::name_ + "::PARASAUR") {}

    core::StationResult ParasaurStation::complete()
    {
        if (use_teleporter_) { find_teleporter(); }
        if (use_teleporter_ ? !TeleportStation::begin() : !BedStation::begin()) {
            return {
                static_cast<BedStation*>(this), false,
                BedStation::get_time_taken<std::chrono::seconds>(), {}
            };
        }

        if (use_teleporter_) {
            asa::entities::local_player->stand_up();
            asa::entities::local_player->crouch();
        }

        int seconds_left = static_cast<int>(load_.count());
        if (last_detected_ != std::chrono::system_clock::time_point() && !util::timedout(
            last_detected_, std::chrono::seconds(10))) {
            seconds_left = std::max(seconds_left, 15);
        }

        do {
            if (check_logs_) {
                asa::interfaces::tribe_manager->update_tribelogs(
                    discord::handle_tribelog_events);
            }
            int sleep = std::min(seconds_left, 10);
            asa::core::sleep_for(std::chrono::seconds(sleep));
            seconds_left -= sleep;
        }
        while (seconds_left > 0);

        if (asa::interfaces::hud->detected_enemy()) {
            send_enemy_detected(real_name_, tag_level_);
            last_detected_ = std::chrono::system_clock::now();
        }
        BedStation::set_completed();
        return {
            static_cast<BedStation*>(this), true,
            BedStation::get_time_taken<std::chrono::seconds>(), {}
        };
    }

    void ParasaurStation::find_teleporter()
    {
        auto start = std::chrono::system_clock::now();
        constexpr int per_turn = 30;
        asa::entities::local_player->set_pitch(90);
        asa::core::sleep_for(std::chrono::milliseconds(500));

        for (int i = 0; i < 360 / per_turn; i++) {
            if (asa::interfaces::HUD::can_teleport()) { break; }
            asa::entities::local_player->turn_right(30);
        }
    }
}

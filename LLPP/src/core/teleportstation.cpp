#include "teleportstation.h"
#include "../discord/embeds.h"
#include "../discord/bot.h"
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/spawnmap.h>
#include "../config/config.h"

namespace llpp::core
{
    TeleportStation::TeleportStation(std::string t_name,
                                     const std::chrono::minutes t_interval)
        : BaseStation(std::move(t_name), t_interval), start_tp_(name_) {}

    TeleportStation::TeleportStation(std::string t_name,
                                     const std::chrono::system_clock::time_point
                                     t_last_completed,
                                     const std::chrono::minutes t_interval)
        : BaseStation(std::move(t_name), t_last_completed, t_interval),
          start_tp_(name_) {}

    StationResult TeleportStation::complete()
    {
        return {this, begin(false), get_time_taken<std::chrono::seconds>(), {}};
    }

    bool TeleportStation::begin(bool check_logs)
    {
        last_started_ = std::chrono::system_clock::now();

        try { asa::entities::local_player->teleport_to(start_tp_, is_default_); }
        catch (const asa::interfaces::DestinationNotFound& e) {
            std::cerr << e.what() << std::endl;
            discord::get_bot()->message_create(
                discord::create_station_disabled_message(name_, e.what()));
            set_state(State::DISABLED);
            return false;
        }
        return true;
    }
}

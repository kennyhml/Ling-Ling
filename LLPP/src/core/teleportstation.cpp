#include "teleportstation.h"
#include "../discord/embeds.h"
#include "../discord/bot.h"
#include "../discord/tribelogs/handler.h"
#include <asapp/core/state.h>
#include <asapp/interfaces/spawnmap.h>
#include <asapp/interfaces/tribemanager.h>

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
        const StationResult res(this, begin(false), get_time_taken<>());
        set_completed();
        return res;
    }

    bool TeleportStation::begin(const bool check_logs)
    {
        last_started_ = std::chrono::system_clock::now();

        try { asa::entities::local_player->teleport_to(start_tp_, get_teleport_flags()); }
        catch (const asa::interfaces::DestinationNotFound& e) {
            std::cerr << e.what() << std::endl;
            discord::get_bot()->message_create(
                discord::create_station_disabled_message(name_, e.what()));
            set_state(State::DISABLED);
            return false;
        }
        if (check_logs) {
            asa::interfaces::tribe_manager->update_tribelogs(
                discord::handle_tribelog_events);
            asa::core::sleep_for(std::chrono::seconds(1));
        }
        return true;
    }

    TeleportFlags TeleportStation::get_teleport_flags() const
    {
        TeleportFlags flags = 0;
        if (is_default_) { flags |= TeleportFlags_UseDefaultOption; }
        if (teleport_unsafe_) { flags |= TeleportFlags_UnsafeLoad; }
        return flags;
    }
}

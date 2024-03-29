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

        try { asa::entities::local_player->teleport_to(start_tp_, flags_); }
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

    void TeleportStation::set_default_destination(const bool is_default)
    {
        if (is_default) { flags_ |= TeleportFlags_UseDefaultOption; }
        else { flags_ &= static_cast<TeleportFlags>(~TeleportFlags_UseDefaultOption); }
    }

    void TeleportStation::set_unsafe_load(const bool unsafe_load)
    {
        if (unsafe_load) { flags_ |= TeleportFlags_UnsafeLoad; }
        else { flags_ &= static_cast<TeleportFlags>(~TeleportFlags_UnsafeLoad); }
    }

}

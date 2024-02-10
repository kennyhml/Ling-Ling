#include "teleportstation.h"

#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/spawnmap.h>


namespace llpp::core
{
    TeleportStation::TeleportStation(std::string t_name,
                                     const std::chrono::minutes t_interval)
        : BaseStation(std::move(t_name), t_interval), start_tp_(name_) {}

    bool TeleportStation::begin()
    {
        last_started_ = std::chrono::system_clock::now();

        try {
            asa::entities::local_player->teleport_to(start_tp_, is_default_);
        } catch (const asa::interfaces::DestinationNotFound& e) {
            std::cerr << e.what() << std::endl;
            // inform via discord that the station was disabled.
            set_state(State::DISABLED);
            return false;
        }
        return true;
    }
}

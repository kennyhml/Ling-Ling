#include "artifactstation.h"

#include <asapp/core/state.h>
#include <asapp/interfaces/hud.h>

namespace llpp::bots
{
    ArtifactStation::ArtifactStation(const std::string& t_name,
                                     const std::chrono::minutes t_interval)
        : TeleportStation(t_name, t_interval),
          artifact_container_(t_name + "::ARTIFACT", 0) {}

    core::StationResult ArtifactStation::complete()
    {

        if (!begin(false)) {
            return {this, false, get_time_taken<>(), {}};
        }
        asa::entities::local_player->stand_up();
        asa::core::sleep_for(500ms);
        asa::entities::local_player->crouch();

        asa::entities::local_player->set_pitch(0);

        const bool up = util::await([this] {
            return asa::entities::local_player->can_access(artifact_container_);
        }, 5s);

        if (up) {
            asa::entities::local_player->access(artifact_container_);
            do {
                artifact_container_.get_inventory()->transfer_all();
            } while (!util::await([this] {
                return !artifact_container_.get_inventory()->is_open();
            }, 5s));
            asa::core::sleep_for(1s);
        }

        return {this, true, get_time_taken<>(), {{"Artifact", up}}};
    }
}

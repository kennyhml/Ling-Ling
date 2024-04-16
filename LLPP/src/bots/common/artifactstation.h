#pragma once
#include "../../core/teleportstation.h"


namespace llpp::bots
{

    class ArtifactStation final : public core::TeleportStation
    {
    public:
        ArtifactStation(const std::string& t_name, std::chrono::minutes t_interval);

        core::StationResult complete() override;

    private:
        asa::structures::Container artifact_container_;
    };
}

#pragma once
#include "config.h"
#include "../../core/teleportstation.h"
#include "../../core/bedstation.h"
#include <asapp/entities/dinoent.h>
#include <asapp/structures/teleporter.h>
#include <asapp/structures/simplebed.h>


namespace llpp::bots::parasaur
{
    class ParasaurStation final : public core::BedStation, public core::TeleportStation
    {
    public:
        explicit ParasaurStation(const std::string& t_real_name,
                                 const ParasaurConfig& t_config);

        core::StationResult complete() override;

    private:
        inline static std::chrono::system_clock::time_point last_detected_;

        std::string real_name_;

        bool use_teleporter_;
        bool check_logs_;
        bool previous_detected_enemy_ = false;

        int tag_level_;
        std::chrono::seconds load_;
        asa::entities::DinoEnt parasaur_;
    };
}

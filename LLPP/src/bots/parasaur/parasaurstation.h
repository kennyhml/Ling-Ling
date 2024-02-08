#pragma once
#include "../../core/basestation.h"

#include <asapp/entities/dinoent.h>
#include <asapp/structures/teleporter.h>
#include <asapp/structures/simplebed.h>


namespace llpp::bots::alert
{

    class ParasaurStation final : core::BaseStation
    {
    public:
        ParasaurStation(std::string t_name, std::chrono::minutes t_interval, bool t_is_bed);

        core::StationResult complete() override;
    private:

        bool is_bed_station_;

        asa::entities::DinoEnt parasaur_;

        asa::structures::Teleporter teleporter_;
        asa::structures::SimpleBed bed_;
    };









}

#pragma once
#include <asapp/structures/simplebed.h>
#include <asapp/structures/container.h>
#include "../../core/basestation.h"

namespace llpp::bots::kitchen
{
    class SapStation final : public core::BaseStation
    {
    public:
        explicit SapStation(std::string t_name, std::chrono::minutes t_interval);

        core::StationResult complete() override;

    private:
        asa::structures::SimpleBed spawn_bed_;
        asa::structures::Container storage_box_;
        asa::structures::Container tap_;

    private:
        bool take_sap() const;
        void put_away_sap() const;
    };
}

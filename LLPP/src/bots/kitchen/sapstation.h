#pragma once
#include <asapp/structures/container.h>
#include "../../core/bedstation.h"

namespace llpp::bots::kitchen
{
    class SapStation final : public core::BedStation
    {
    public:
        SapStation(std::string t_name,
                   std::chrono::system_clock::time_point t_last_completed,
                   std::chrono::minutes t_interval);

        core::StationResult complete() override;

    private:
        [[nodiscard]] bool take_sap() const;
        void put_away_sap();

        asa::structures::Container storage_box_;
        asa::structures::Container tap_;

        int storage_box_slots_ = 0;
    };
}

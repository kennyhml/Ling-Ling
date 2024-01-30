#pragma once

#include "../../core/basestation.h"
#include <asapp/structures/craftingstation.h>
#include <asapp/structures/simplebed.h>
#include <asapp/structures/dedicatedstorage.h>


namespace llpp::bots::crafting
{
    class ARBStation final : public core::BaseStation
    {
    public:
        ARBStation(std::string t_name, std::chrono::minutes t_interval);

        core::StationResult complete() override;

        [[nodiscard]] cv::Mat get_last_dedi_ss() {return last_dedi_screenshot_; }

    private:
        void empty();

        void requeue();

        void put_overproduce_back();

        void await_deposited();

        asa::structures::SimpleBed bed_;
        asa::structures::CraftingStation fabricator_;

        cv::Mat last_dedi_screenshot_;
        bool was_still_crafting_ = false;
        int last_amount_taken_ = 0;

        std::array<asa::structures::Container, 3> dedis_{
                asa::structures::Container("DEDI01", 1800),
                asa::structures::Container("DEDI02", 1800),
                asa::structures::Container("DEDI03", 1800),
        };

    };

}
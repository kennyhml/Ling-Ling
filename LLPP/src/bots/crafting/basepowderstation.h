#pragma once

#include <string>
#include <chrono>
#include "asapp/structures/craftingstation.h"
#include "../../core/bedstation.h"

namespace llpp::bots::crafting
{
    class StationFullError : public std::exception
    {
    public:
        explicit StationFullError(std::string t_info) : info_(std::move(t_info)) {}

        const char* what() const override { return info_.c_str(); }

    private:
        std::string info_;
    };

    class BasePowderStation : public core::BedStation
    {
    public:
        BasePowderStation(const std::string& t_name, std::chrono::minutes t_interval,
                          const asa::items::Item& t_to_craft, int t_amount_per_queue);


        [[nodiscard]] const cv::Mat& get_last_dedi_ss() const
        {
            return last_dedi_screenshot_;
        }

    protected:
        void empty();

        void requeue();

        void await_deposited();

        void put_overproduce_back();

        int amount_to_queue_;
        bool was_still_crafting_ = false;

        asa::items::Item to_craft_;
        cv::Mat last_dedi_screenshot_;

        std::chrono::system_clock::time_point crafting_start_;
        asa::structures::CraftingStation chem_bench_;

        std::array<asa::structures::Container, 3> dedis_{
                asa::structures::Container("DEDI01", 1800),
                asa::structures::Container("DEDI02", 1800),
                asa::structures::Container("DEDI03", 1800),
        };
    };
}

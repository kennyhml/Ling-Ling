#pragma once
#include <chrono>
#include <string>
#include <unordered_map>

namespace llpp::core
{
    class BaseStation;

    struct StationResult
    {
        StationResult(BaseStation* t_station, const bool t_succeeded,
                      const std::chrono::seconds t_time_taken,
                      const std::unordered_map<std::string, int>& t_obtained_items)
            : station_(t_station), success_(t_succeeded),
              time_taken_(t_time_taken), obtained_items_(t_obtained_items)
        {
        }

        [[nodiscard]] bool get_success() const { return success_; }
        [[nodiscard]] std::chrono::seconds get_time_taken() const { return time_taken_; }
        [[nodiscard]] const BaseStation* get_station() const { return station_; }

        [[nodiscard]] const std::unordered_map<std::string, int>&
        get_obtained_items() const { return obtained_items_; }

    private:
        BaseStation* station_;
        bool success_;
        std::chrono::seconds time_taken_;
        std::unordered_map<std::string, int> obtained_items_;
    };
}

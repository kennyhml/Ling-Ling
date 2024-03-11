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
                      const std::unordered_map<std::string, int>& t_obtained_items = {})
            : station(t_station), success(t_succeeded), time_taken(t_time_taken),
              obtained_items(t_obtained_items) {}

        BaseStation* station;
        bool success;
        std::chrono::seconds time_taken;
        std::unordered_map<std::string, int> obtained_items;
    };
}

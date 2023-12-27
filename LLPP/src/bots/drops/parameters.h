#pragma once
#include <string>
#include <chrono>

namespace llpp::bots::drops
{
    struct CrateStationParams final
    {
        std::string name;
        bool is_bed_station;
        std::chrono::minutes interval;
        std::chrono::seconds max_render_time;
    };
}

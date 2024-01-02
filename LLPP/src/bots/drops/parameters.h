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

    struct CrateManagerConfig final
    {
        std::string prefix;
        std::string grouped_crates_raw{"{}"};

        int interval{15};
        int render_for{2};

        bool uses_teleporters{false};
        bool overrule_reroll_mode{false};
        bool allow_partial_completion{false};
    };
}

#pragma once
#include <string>
#include <chrono>

namespace llpp::bots::drops
{
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

#pragma once
#include <string>

namespace llpp::bots::drops
{
    struct CrateManagerConfig final
    {
        std::string prefix;
        std::string grouped_crates_raw{"{}"};

        int interval{15};
        int render_group_for{2};
        int render_align_for{5};
        int render_initial_for{3};

        bool no_reroll{false};
        bool allow_partial_completion{false};
        bool disabled{false};
    };
}

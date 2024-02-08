#pragma once
#include <string>

namespace llpp::bots::parasaur
{
    struct ParasaurConfig final
    {
        std::string name;

        int interval{15};

        bool is_teleporter{false};
        bool disabled{false};
    };
}

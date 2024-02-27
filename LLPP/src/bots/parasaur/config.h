#pragma once
#include <string>
#include <functional>

namespace llpp::bots::parasaur
{
    struct ParasaurConfig final
    {
        std::string name;

        int interval{15};
        int load_for{5};
        int alert_level{0};
        int last_completed{0};

        bool check_logs{false};
        bool is_teleporter{false};
        bool disabled{false};

        std::function<void()> on_changed = nullptr;
    };
}
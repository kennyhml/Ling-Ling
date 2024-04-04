#pragma once
#include <string>
#include <functional>

namespace llpp::bots::farm
{
    struct FarmConfig final
    {
        std::string prefix;
        std::string type{"METAL"};

        int64_t last_completed{0};

        int32_t num_stations{0};
        int32_t interval{120};

        bool disabled{false};
        bool check_logs{true};

        std::function<void()> on_changed = nullptr;
    };
}

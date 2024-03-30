#pragma once
#include <string>

namespace llpp::bots::metal
{
    struct MetalManagerConfig final
    {
        std::string prefix;

        int64_t last_completed{0};

        int32_t num_stations{0};
        int32_t interval{120};

        bool disabled{false};
        bool check_logs{true};

        std::function<void()> on_changed = nullptr;
    };
}

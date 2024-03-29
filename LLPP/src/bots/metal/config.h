#pragma once
#include <string>

namespace llpp::bots::metal
{
    struct MetalManagerConfig final
    {
        std::string prefix;

        int64_t last_completed;

        uint32_t num_stations;
        uint32_t interval_minutes_;

        bool disabled;

        std::function<void()> on_changed = nullptr;
    };
}
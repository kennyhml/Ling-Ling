#pragma once
#include <string>

namespace llpp::bots::metal
{

    struct MetalManagerConfig final
    {
        std::string prefix;

        int num_stations;
        bool disabled;

        std::function<void()> on_changed = nullptr;
    };

}
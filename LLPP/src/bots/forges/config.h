#pragma once
#include <string>
#include <cstdint>
#include <functional>

namespace llpp::bots::forges
{
    struct ForgeConfig final
    {
        std::string prefix;
        std::string cooking_material;

        int64_t last_filled{0};
        int32_t view_difference{0};

        std::function<void()> on_changed = nullptr;
    };
}

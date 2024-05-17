#pragma once
#include "../../core/basestation.h"

namespace llpp::bots::parasaur
{
    struct ParasaurStationResult final
    {
        bool ran{false};
        core::BaseStation* station{nullptr};
        std::chrono::system_clock::time_point last_detection;
        std::string real_name_;
    };
}

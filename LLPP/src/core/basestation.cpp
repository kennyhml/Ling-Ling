#include "basestation.h"

namespace llpp::core
{
    BaseStation::BaseStation(std::string t_name, const std::chrono::minutes t_interval) :
        name_(std::move(t_name)), completion_interval_(t_interval) {}

    bool BaseStation::is_ready() const
    {
        return !disabled_ && (std::chrono::system_clock::now() - last_completed_) >
            completion_interval_;
    }

    void BaseStation::set_completed()
    {
        times_completed_++;
        last_completed_ = std::chrono::system_clock::now();
    }
}

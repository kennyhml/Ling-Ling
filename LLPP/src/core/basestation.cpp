#include "basestation.h"
#include <asapp/util/util.h>

namespace llpp::core
{
    BaseStation::BaseStation(std::string t_name, const std::chrono::minutes t_interval) :
        name_(std::move(t_name)), completion_interval_(t_interval) {}

    BaseStation::BaseStation(std::string t_name,
                             const std::chrono::system_clock::time_point t_last_completed,
                             const std::chrono::minutes t_interval)
        : name_(std::move(t_name)), completion_interval_(t_interval),
          last_completed_(t_last_completed) {}


    bool BaseStation::is_ready()
    {
        switch (state_) {
        case State::DISABLED: return false;
        case State::SUSPENDED:
        {
            if (!is_suspension_over()) { return false; }
            state_ = State::ENABLED;
            return true;
        }
        case State::ENABLED: return is_interval_over();
        }
        return false;
    }

    void BaseStation::set_completed()
    {
        times_completed_++;
        last_completed_ = std::chrono::system_clock::now();
    }

    void BaseStation::suspend_for(const std::chrono::minutes how_long)
    {
        state_ = State::SUSPENDED;
        suspended_at_ = std::chrono::system_clock::now();
        suspended_for_ = how_long;
    }
}

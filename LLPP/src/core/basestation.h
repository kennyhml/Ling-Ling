#pragma once
#include "stationresult.h"
#include <chrono>
#include <string>

namespace llpp::core
{
    class BaseStation
    {
    public:
        virtual StationResult complete() = 0;

        [[nodiscard]] std::string get_name() const { return name_; }
        [[nodiscard]] int get_times_completed() const { return times_completed_; }
        [[nodiscard]] virtual bool is_ready() const;

        [[nodiscard]] virtual bool get_disabled() const { return disabled_; }
        void virtual set_disabled(const bool disabled) { disabled_ = disabled; }

        [[nodiscard]] std::chrono::system_clock::time_point get_last_completion() const
        {
            return last_completed_;
        }

        [[nodiscard]] std::chrono::system_clock::time_point get_next_completion() const
        {
            return last_completed_ + completion_interval_;
        }

        [[nodiscard]] std::chrono::minutes get_completion_interval() const
        {
            return completion_interval_;
        }

    protected:
        BaseStation(std::string t_name, std::chrono::minutes t_interval);

        std::string name_;
        std::chrono::minutes completion_interval_;
        std::chrono::system_clock::time_point last_completed_;

        int times_completed_ = 0;
        bool disabled_ = false;

        void set_completed();
    };
}

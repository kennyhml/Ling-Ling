#pragma once
#include "stationresult.h"
#include <chrono>
#include <string>

namespace llpp::core
{
    class BaseStation
    {
    public:
        [[nodiscard]] std::string get_name() const { return name_; }
        [[nodiscard]] int get_times_completed() const { return times_completed_; }
        [[nodiscard]] virtual bool is_ready() const;

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

    public:
        enum Status
        {
            UNKNOWN,
            IN_COMPLETION,
            ON_COOLDOWN,
            PENDING,
            DISABLED
        };

    public:
        virtual ~BaseStation() = default;
        virtual StationResult complete() = 0;
        BaseStation(const BaseStation&) = default;
        BaseStation& operator=(const BaseStation&) = default;
        BaseStation(BaseStation&&) = default;
        BaseStation& operator=(BaseStation&&) = default;

    protected:
        BaseStation(std::string t_name, std::chrono::minutes t_interval);

        std::string name_;
        std::chrono::minutes completion_interval_;
        std::chrono::system_clock::time_point last_completed_;

        int times_completed_;
        Status status_;

        void set_completed();
    };
}

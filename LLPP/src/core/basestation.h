#pragma once
#include "stationresult.h"
#include <chrono>
#include <string>
#include <asapp/structures/simplebed.h>
#include <asapp/util/util.h>

namespace llpp::core
{
    class BaseStation
    {
    public:
        enum class State : int32_t
        {
            ENABLED,
            DISABLED,
            SUSPENDED,
        };

    public:
        /**
         * @brief Completes this station, is_ready must be checked beforehand.
         *
         * @return The result of the completion as a StationResult.
         */
        virtual StationResult complete() = 0;

        /**
         * @brief Gets the name of the station.
         */
        [[nodiscard]] std::string get_name() const { return name_; }

        /**
         * @brief Gets the amount of times this station has been completed.
         */
        [[nodiscard]] int get_times_completed() const { return times_completed_; }

        /**
         * @brief Checks whether the station is ready to be completed.
         */
        [[nodiscard]] virtual bool is_ready();

        /**
         * @brief Gets the current state of the station.
         */
        [[nodiscard]] State get_state() const { return state_; }

        /**
         * @brief Sets the state of the station.
         * @param state The new state to set the station to.
         */
        void virtual set_state(const State state) { state_ = state; }

        /**
         * @brief Gets the last completion time point of the station.
         */
        [[nodiscard]] std::chrono::system_clock::time_point get_last_completion() const
        {
            return last_completed_;
        }

        /**
         * @brief Gets the next completion time point of the station.
         */
        [[nodiscard]] std::chrono::system_clock::time_point get_next_completion() const
        {
            return last_completed_ + completion_interval_;
        }

        /**
         * @brief Gets the completion interval of the station.
         */
        [[nodiscard]] std::chrono::minutes get_completion_interval() const
        {
            return completion_interval_;
        }

        /**
         * @brief Suspends this station for a given duration.
         * @param how_long The duration to suspend the station for.
         */
        void suspend_for(std::chrono::minutes how_long);

    protected:
        BaseStation(std::string t_name, std::chrono::minutes t_interval);
        BaseStation(std::string t_name,
                    std::chrono::system_clock::time_point t_last_completed,
                    std::chrono::minutes t_interval);
        /**
         * @brief Begins the station by setting the start time point and spawning there.
         *
         * @returns True if the station was started successfully, false otherwise.
         */
        [[nodiscard]] virtual bool begin() = 0;

        template <typename T>
        T get_time_taken() const { return util::get_elapsed<T>(last_started_); }

        /**
         * @brief To be called when the station was completed, increments the completion
         * count and sets the last completion time.
         */
        void set_completed();

        /**
         * @brief Checks whether the suspension time has passed.
         */
        [[nodiscard]] bool is_suspension_over() const
        {
            return util::get_elapsed<std::chrono::minutes>(suspended_at_) >
                suspended_for_;
        }

        /**
         * @brief Checks whether the interval of the station has elapsed.
         */
        [[nodiscard]] bool is_interval_over() const
        {
            return util::get_elapsed<std::chrono::minutes>(last_completed_) >
                completion_interval_;
        }

        State state_ = State::ENABLED;
        std::string name_;
        std::chrono::minutes completion_interval_;
        std::chrono::minutes suspended_for_{0};

        std::chrono::system_clock::time_point last_completed_;
        std::chrono::system_clock::time_point suspended_at_;
        std::chrono::system_clock::time_point last_started_;

        int times_completed_ = 0;
        bool disabled_ = false;

        inline static asa::structures::SimpleBed reset_bed_{"RESET BED"};
    };
}

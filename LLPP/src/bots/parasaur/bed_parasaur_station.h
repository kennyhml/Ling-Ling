#pragma once
#include "config.h"
#include "../../core/bedstation.h"
#include <asapp/entities/dinoentity.h>

namespace llpp::bots::parasaur
{
    class BedParasaurStation final : public core::BedStation
    {
    public:
        [[nodiscard]] std::string get_real_name() const { return real_name_; }

        BedParasaurStation(std::string t_real_name, ParasaurConfig* t_config);

        core::StationResult complete() override;

        bool is_ready() override
        {
            return !config_->disabled && BedStation::is_ready();
        }

        /**
         * @return Gets the time point of the last enemy detection at this station.
         */
        [[nodiscard]] std::chrono::system_clock::time_point get_last_alert() const
        {
            return last_detection_;
        };

    private:
        static bool has_lingering_ping();

        inline static std::chrono::system_clock::time_point last_detected_;

        ParasaurConfig* config_;

        std::chrono::system_clock::time_point last_detection_;
        std::string real_name_;
        asa::entities::DinoEntity parasaur_;
    };
}

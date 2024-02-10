#pragma once
#include "../../core/istationmanager.h"
#include "parasaurstation.h"

namespace llpp::bots::parasaur
{
    class ParasaurManager final : public core::IStationManager
    {
    public:
        ParasaurManager();

        bool run() override;
        [[nodiscard]] bool is_ready_to_run() override;
        [[nodiscard]] std::chrono::minutes get_time_left_until_ready() const override;

    private:
        /**
         * @brief Travels to the start tp to start the tp stations.
         */
        void go_to_start_tp() const;

        std::chrono::system_clock::time_point last_completed_;

        std::vector<std::unique_ptr<ParasaurStation>> bed_stations_;
        std::vector<std::unique_ptr<ParasaurStation>> tp_stations_;
    };
}

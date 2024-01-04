#pragma once
#include "../../core/istationmanager.h"
#include "sapstation.h"
#include <memory>

namespace llpp::bots::kitchen
{
    class SapManager final : public core::IStationManager
    {
    public:
        SapManager(std::string t_prefix, int t_num_stations,
                   std::chrono::minutes t_interval);

        bool run() override;
        bool is_ready_to_run() const override;
        std::chrono::minutes get_time_left_until_ready() const override;

    private:
        std::vector<std::unique_ptr<SapStation>> stations_;
    };
}

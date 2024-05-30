#pragma once
#include "../../core/istationmanager.h"
#include "phoenixstation.h"
#include <memory>

namespace llpp::bots::phoenix
{
    class PhoenixManager final : public core::IStationManager
    {
    public:
        PhoenixManager();

        bool run() override;
        bool is_ready_to_run() override;
        [[nodiscard]] std::chrono::minutes get_time_left_until_ready() const override;

    private:
        std::vector<std::unique_ptr<PhoenixStation>> phoenix_stations_;
    };
}

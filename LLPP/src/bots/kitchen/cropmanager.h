#pragma once
#include "../../core/istationmanager.h"
#include "cropstation.h"
#include <memory>

namespace llpp::bots::kitchen
{
    class CropManager final : public core::IStationManager
    {
    public:
        CropManager();

        bool run() override;
        bool is_ready_to_run() const override;
        std::chrono::minutes get_time_left_until_ready() const override;

    private:
        std::vector<std::unique_ptr<CropStation>> longrass_stations_;
        std::vector<std::unique_ptr<CropStation>> citronal_stations_;
        std::vector<std::unique_ptr<CropStation>> rockarrot_stations_;
        std::vector<std::unique_ptr<CropStation>> savoroot_stations_;
    };
}

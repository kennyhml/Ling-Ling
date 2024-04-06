#pragma once
#include "forgestation.h"
#include "../../core/istationmanager.h"


namespace llpp::bots::forges
{

    class ForgeManager final : public core::IStationManager
    {
    public:
        ForgeManager();

        bool run() override;

        [[nodiscard]] bool is_ready_to_run() override;
        [[nodiscard]] std::chrono::minutes get_time_left_until_ready() const override;

    private:
        std::shared_ptr<std::vector<UnloadStation>> unloads_;
        std::shared_ptr<std::vector<LoadupStation>> loadups_;

        std::vector<std::unique_ptr<ForgeStation>> stations_;
    };



}

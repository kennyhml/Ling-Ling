#pragma once
#include "woodstation.h"
#include "../common/config.h"
#include "../common/collectstation.h"
#include "../common/unloadstation.h"
#include "../common/mountstation.h"
#include "../../../core/istationmanager.h"
#include "../../drops/bed_crate_station.h"

namespace llpp::bots::farm
{
    class WoodManager final : public core::IStationManager
    {
    public:
        explicit WoodManager(FarmConfig* t_config);

        bool run() override;

        [[nodiscard]] bool is_ready_to_run() override;

        [[nodiscard]] std::chrono::minutes get_time_left_until_ready() const override;

    private:

        void get_chainsaw() const;

        void unload();

        FarmConfig* config_;

        core::BedStation start_bed_;
        core::TeleportStation start_tp_;
        core::TeleportStation unload_tp;

        asa::structures::CraftingStation fabricator_;

        std::vector<std::unique_ptr<WoodStation>> stations_;
    };
}

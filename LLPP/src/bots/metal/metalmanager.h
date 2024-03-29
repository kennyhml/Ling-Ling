#pragma once
#include <asapp/structures/dedicatedstorage.h>

#include "config.h"
#include "metalstation.h"
#include "../../core/bedstation.h"
#include "../../core/istationmanager.h"


namespace llpp::bots::metal
{
    class MetalManager final : core::IStationManager
    {
    public:
        explicit MetalManager(MetalManagerConfig* t_config);

        bool run() override;

        [[nodiscard]] bool is_ready_to_run() override;

        [[nodiscard]] std::chrono::minutes get_time_left_until_ready() const override;

    private:
        bool mount_anky();

        void unload();

        void collect();

        MetalManagerConfig* config_;

        core::BedStation mount_bed_;
        core::BedStation collect_bed_;

        core::TeleportStation unload_tp_;
        core::TeleportStation start_tp_;

        std::shared_ptr<asa::entities::DinoEntity> anky_;

        std::vector<std::unique_ptr<MetalStation>> stations_;
        std::vector<asa::structures::DedicatedStorage> dedis_;
    };
}

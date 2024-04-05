#pragma once
#include <asapp/entities/dinoentity.h>
#include "../../../core/bedstation.h"
#include "../../common/renderstation.h"

namespace llpp::bots::farm
{
    class MountStation final : public core::BedStation
    {
    public:
        MountStation(const std::string& t_name,
                              std::shared_ptr<asa::entities::DinoEntity> t_anky,
                              std::unique_ptr<RenderStation> t_render_station = nullptr,
                              std::unique_ptr<BedStation> t_reposition_bed = nullptr);

        core::StationResult complete() override;

    private:
        void whip_mount();

        std::shared_ptr<asa::entities::DinoEntity> anky_;
        std::unique_ptr<RenderStation> render_station_;
        std::unique_ptr<BedStation> reposition_bed_;




    };
}

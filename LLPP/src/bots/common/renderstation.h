#pragma once
#include "../../core/bedstation.h"

namespace llpp::bots
{
    class RenderStation final : public core::BedStation
    {
    public:
        RenderStation(std::string t_name, std::chrono::seconds t_load_for);

        core::StationResult complete() override;

    private:
        std::chrono::seconds render_duration_;
        inline static BedStation gateway_{"RENDER::GATEWAY", std::chrono::minutes(0)};
    };
};

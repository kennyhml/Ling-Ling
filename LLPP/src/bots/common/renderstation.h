#pragma once
#include "../../core/bedstation.h"

namespace llpp::bots
{
    class RenderStation final : public core::BedStation
    {
    public:
        RenderStation(std::string t_name, std::chrono::seconds t_load_for, bool t_src_reconnect = false);

        core::StationResult complete() override;

    private:
        std::chrono::seconds render_duration_;
        bool src_reconnect_;

        inline static BedStation gateway_{"RENDER::GATEWAY", std::chrono::minutes(0)};
    };
};

#pragma once
#include "../../core/istationmanager.h"
#include "../common/renderstation.h"
#include "pastestation.h"
#include <memory>

namespace llpp::bots::paste
{
    class PasteManager final : public core::IStationManager
    {
    public:
        PasteManager();

        bool run() override;
        [[nodiscard]] bool is_ready_to_run() override;
        [[nodiscard]] std::chrono::minutes get_time_left_until_ready() const override;

    private:
        std::vector<std::unique_ptr<PasteStation>> paste_stations_;

        RenderStation render_station_{
            "PASTE::RENDER::SRC", std::chrono::seconds(10)
        };

        [[nodiscard]] bool is_paste_rendered() const;
    };
}

#pragma once
#include "../../core/istationmanager.h"
#include "../render/renderstation.h"
#include "pastestation.h"
#include <memory>

#include "grindstation.h"

namespace llpp::bots::paste
{
    class PasteManager final : public core::IStationManager
    {
    public:
        PasteManager();

        bool run() override;
        [[nodiscard]] bool is_ready_to_run() const override;
        [[nodiscard]] std::chrono::minutes get_time_left_until_ready() const override;
        [[nodiscard]] const PasteStation* peek_station(int index) const;

    private:
        std::vector<std::unique_ptr<PasteStation>> paste_stations_;
        std::vector<std::unique_ptr<GrindStation>> grind_stations_;

        render::RenderStation render_station_{
            "PASTE::RENDER::SRC", std::chrono::seconds(10)
        };

        [[nodiscard]] bool is_paste_rendered() const;
    };
}

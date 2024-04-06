#pragma once
#include <asapp/structures/dedicatedstorage.h>
#include "../../core/teleportstation.h"

namespace llpp::bots::forges
{
    class LoadupStation final : public core::TeleportStation
    {
    public:
        explicit LoadupStation(const std::string& t_name, std::string t_material);

        core::StationResult complete() override;

        [[nodiscard]] const std::string& get_material() const { return material_; }

    private:

        bool get_slotcap();

        std::string material_;
        std::array<asa::structures::DedicatedStorage, 6> dedis_;
    };
}

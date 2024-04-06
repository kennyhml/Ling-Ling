#pragma once
#include <asapp/structures/dedicatedstorage.h>

#include "../../core/teleportstation.h"

namespace llpp::bots::forges
{
    class UnloadStation final : public core::TeleportStation
    {
    public:
        explicit UnloadStation(const std::string& t_name, std::string t_material);

        core::StationResult complete() override;

        [[nodiscard]] const std::string& get_material() const { return material_; }

    private:
        std::string material_;
        std::array<asa::structures::DedicatedStorage, 6> dedis_;
    };
}

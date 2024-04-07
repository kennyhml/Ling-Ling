#pragma once
#include <asapp/structures/dedicatedstorage.h>

#include "lootresult.h"
#include "../../core/teleportstation.h"

namespace llpp::bots::drops
{
    class DropoffStation final : core::TeleportStation
    {
    public:
        DropoffStation();

        core::StationResult complete() override;

        void set_to_sort(const std::vector<LootResult>& sort) { to_sort_ = sort; }

    private:
        enum class VaultType : int32_t
        {
            BLUEPRINTS,
            SADDLES,
            GRINDABLES
        };

    private:
        void sort_into_vault(VaultType vault);

        void sort_into_dedi(const std::string& item);

        void deposit_into_fridge();

        asa::structures::SimpleBed align_bed_;
        asa::structures::Container fridge_;

        std::array<asa::structures::Container, 3> vaults_;

        std::vector<LootResult> to_sort_;
    };
}

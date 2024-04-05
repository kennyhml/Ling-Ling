#pragma once
#include <asapp/structures/dedicatedstorage.h>
#include "../../../core/bedstation.h"

namespace llpp::bots::farm
{
    class CollectStation final : public core::BedStation
    {
    public:
        explicit CollectStation(const std::string& t_name);

        /**
         * @brief Completes the station by spawning there and collecting the resources.
         *
         * @return The result of the station completion.
         */
        core::StationResult complete() override;

    private:
        /**
         * @brief Gets the whip from the vault, if broken it will be repaired by pulling
         * from nearby dedis while attempting to equip it.
         */
        void get_whip() const;

        /**
         * @brief Puts the whip back into the vault.
         */
        void put_away_whip() const;

        /**
         * @brief Deposits the collected metal into the dedis.
         */
        void deposit() const;

        asa::structures::Container vault_;
        std::vector<asa::structures::DedicatedStorage> dedis_;
    };
}

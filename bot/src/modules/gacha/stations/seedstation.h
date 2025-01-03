#pragma once
#include "core/teleportstation.h"

namespace lingling::gacha
{
    enum seeding_flag : int32_t
    {
        SEEDING_INVENTORY_CLEAROUT = 1 << 1,
        SEEDING_RESTOCK_FOR_NEXT = 1 << 2,
        SEEDING_NO_BERRY_TRANSFER = 1 << 3,
    };

    using seeding_flags_t = int32_t;

    class seedstation final : public teleport_station
    {
    public:
        seedstation(const std::string& t_name,
                    std::shared_ptr<asa::teleporter> t_teleporter);

        /**
         * @brief Completes the station by teleporting to it (if neccessary) and using the
         * iguanodon with the berries from the inventory to generate 150 slots of seeds.
         *
         * The station ends with the view angles in the same position as it was started with
         * and the iguanodon inventory closed. The produced seeds will be in the players
         * inventory.
         */
        void complete() override;

        /**
         * @brief Sets flags to control how the station should be completed, specifically
         * the seeding part of the process.
         */
        void set_seeding_flags(seeding_flags_t flags);

    private:
        void seed();

        void take_berries() const;

        // Iguanodon is unique to the station and not shared
        asa::dino_entity iguanodon_;
        seeding_flags_t flags_ = 0;
    };
}

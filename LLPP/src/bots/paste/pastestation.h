#pragma once
#include "../../core/bedstation.h"
#include <asapp/entities/dinoent.h>
#include <asapp/structures/dedicatedstorage.h>

namespace llpp::bots::paste
{
    class PasteStation final : public core::BedStation
    {
    public:
        PasteStation(std::string t_name, std::chrono::minutes t_interval);

        core::StationResult complete() override;

    private:
        std::array<asa::entities::DinoEnt, 6> achatinas{
            asa::entities::DinoEnt("ACHATINA01"), asa::entities::DinoEnt("ACHATINA02"),
            asa::entities::DinoEnt("ACHATINA03"), asa::entities::DinoEnt("ACHATINA04"),
            asa::entities::DinoEnt("ACHATINA05"), asa::entities::DinoEnt("ACHATINA06"),
        };

        asa::structures::DedicatedStorage dedi_;

        bool empty(asa::entities::DinoEnt& achatina);
        void empty_all();
        int deposit_paste();

        bool turn_until_deposit() const;
    };
}

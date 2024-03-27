#pragma once
#include "../../core/bedstation.h"
#include <asapp/entities/dinoentity.h>
#include <asapp/structures/dedicatedstorage.h>

namespace llpp::bots::paste
{
    class PasteStation final : public core::BedStation
    {
    public:
        PasteStation(std::string t_name,
                     std::chrono::system_clock::time_point t_last_completed,
                     std::chrono::minutes t_interval);

        core::StationResult complete() override;

    private:
        std::array<asa::entities::DinoEntity, 6> achatinas{
            asa::entities::DinoEntity("ACHATINA01"),
            asa::entities::DinoEntity("ACHATINA02"),
            asa::entities::DinoEntity("ACHATINA03"),
            asa::entities::DinoEntity("ACHATINA04"),
            asa::entities::DinoEntity("ACHATINA05"),
            asa::entities::DinoEntity("ACHATINA06"),
        };

        asa::structures::DedicatedStorage dedi_;

        bool empty(asa::entities::DinoEntity& achatina);
        void empty_all();
        int deposit_paste();

        bool turn_until_deposit() const;
    };
}

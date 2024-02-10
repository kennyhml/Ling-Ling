#pragma once
#include "../../core/bedstation.h"
#include <asapp/structures/container.h>
#include <asapp/structures/dedicatedstorage.h>


namespace llpp::bots::crafting
{
    class ForgeStation final : public core::BedStation
    {
    public:
        ForgeStation(std::string t_name, std::chrono::minutes t_interval);

        core::StationResult complete() override;

    private:

        void empty();
        void fill();

        asa::items::Item* last_known_item_ = nullptr;
        int last_amount_taken_ = 0;
        int last_amount_added_ = 0;

        asa::structures::DedicatedStorage dedi_raw_;
        asa::structures::DedicatedStorage dedi_cooked_;

        asa::structures::Container forge_;
    };

}
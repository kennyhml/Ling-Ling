#pragma once
#include <asapp/items/items.h>
#include <asapp/structures/mediumcropplot.h>
#include <asapp/structures/simplebed.h>
#include "../../core/basestation.h"

namespace llpp::bots::kitchen
{
    class CropStation final : public core::BaseStation
    {
    public:
        enum CropType
        {
            LONGRASS,
            SAVOROOT,
            ROCKARROT,
            CITRONAL,
        };

    public:
        CropStation(std::string t_name, bool t_left_aligned, CropType crop,
                    std::chrono::minutes t_interval);

        core::StationResult complete() override;

    private:
        bool left_aligned_;
        asa::items::Item* crop_;
        asa::items::Item* seed_;

        asa::structures::SimpleBed spawn_bed_;
        asa::structures::Container fridge_;
        asa::structures::Container vault_;
        std::array<asa::structures::MediumCropPlot, 6> crop_plots_;

    private:
        int get_slots_to_refill();

        void grab_fertilizer();
        void deposit_fertilizer();

        void put_crops_in_fridge(int& fridge_slots_out);
        void empty(const asa::structures::MediumCropPlot&, int& current_slots,
                   bool count);
        void get_crops(int how_many_slots);
        void turn_to_crop_plots() const;
    };
}

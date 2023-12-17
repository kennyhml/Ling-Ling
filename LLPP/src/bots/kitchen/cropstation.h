#pragma once
#include <asapp/items/items.h>
#include <asapp/structures/mediumcropplot.h>
#include <asapp/structures/simplebed.h>
#include "../../core/basestation.h"

namespace llpp::bots::kitchen
{
    class CropStation : public core::BaseStation
    {
    public:
        CropStation(std::string t_name, bool t_left_aligned, asa::items::Item* t_crop,
                    std::chrono::minutes t_interval);

        core::StationResult complete() override;

    private:
        bool left_aligned_;
        asa::items::Item* crop_;
        asa::structures::SimpleBed spawn_bed_;
        asa::structures::Container fridge_;
        std::array<asa::structures::MediumCropPlot, 6> crop_plots_;

    private:
        int get_slots_to_refill();

        void empty(const asa::structures::MediumCropPlot&, int& current_slots);
        void get_crops(int how_many_slots);
        void turn_to_crop_plots() const;
    };
}

#pragma once
#include <asapp/structures/mediumcropplot.h>
#include "../../core/bedstation.h"

namespace llpp::bots::kitchen
{
    class CropStation final : public core::BedStation
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
        CropStation(std::string t_name, CropType crop, std::chrono::minutes t_interval);

        core::StationResult complete() override;

    private:
        enum CropAlignment
        {
            LEFT,
            RIGHT,
            UNKNOWN
        };

        CropAlignment aligned_ = UNKNOWN;
        asa::items::Item* crop_;
        asa::items::Item* seed_;

        asa::structures::Container fridge_;
        asa::structures::Container vault_;
        std::array<asa::structures::MediumCropPlot, 6> crop_plots_;

    private:
        int get_slots_to_refill();

        void grab_fertilizer() const;
        void deposit_fertilizer() const;

        void put_crops_in_fridge(int& fridge_slots_out);
        void empty(const asa::structures::MediumCropPlot&, int& current_slots,
                   bool count) const;
        void get_crops(int how_many_slots);
        void turn_to_crop_plots() const;
    };
}

#include "cropstation.h"
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>

namespace llpp::bots::kitchen
{
    CropStation::CropStation(std::string t_name, const bool t_left_aligned,
                             asa::items::Item* t_crop,
                             const std::chrono::minutes t_interval) :
        BaseStation(std::move(t_name), t_interval), left_aligned_(t_left_aligned),
        crop_(t_crop), spawn_bed_(name_), fridge_(name_, 80),
        crop_plots_({
            {"PLOT01"}, {"PLOT02"}, {"PLOT03"}, {"PLOT04"}, {"PLOT05"}, {"PLOT06"}
        })
    {
    };

    core::StationResult CropStation::complete()
    {
        const auto start = std::chrono::system_clock::now();
        asa::entities::local_player->fast_travel_to(spawn_bed_);

        const int slots_needed = get_slots_to_refill();
        if (slots_needed == 0) {
            std::cout << "[+] Fridge is completely filled!\n";
        }

        get_crops(slots_needed);

        return core::StationResult(this, true, std::chrono::seconds(0), {});
    }

    int CropStation::get_slots_to_refill()
    {
        std::cout << "[+] Checking current amount of crops...\n";
        asa::entities::local_player->access(fridge_);
        std::cout << "\t[-] Fridge accessed, restacking crops... ";
        fridge_.inventory->auto_stack();
        std::cout << "Done.\n";

        // if the server is laggy the slots my need some time to load
        asa::core::sleep_for(std::chrono::seconds(2));
        std::cout << "\t[-] Counting current slots of crops... ";
        const int filled_slots = fridge_.get_slot_count();
        std::cout << filled_slots << "\\" << fridge_.get_max_slots() << ".\n";

        fridge_.inventory->close();
        return fridge_.get_max_slots() - filled_slots;
    }

    void CropStation::turn_to_crop_plots() const
    {
        if (left_aligned_) { asa::entities::local_player->turn_left(); }
        else { asa::entities::local_player->turn_right(); }
        asa::core::sleep_for(std::chrono::milliseconds(500));
    }

    void CropStation::empty(const asa::structures::MediumCropPlot& plot,
                            int& current_slots)
    {
        asa::entities::local_player->access(plot);
        plot.inventory->transfer_all(crop_);

        while (plot.inventory->has(crop_)) {
        }

        asa::entities::local_player->get_inventory()->count_stacks(
            crop_, current_slots, true);

        std::cout << "[+] Now " << current_slots << " crops...\n";
        asa::entities::local_player->get_inventory()->close();
        asa::core::sleep_for(std::chrono::milliseconds(300));
    }
    
    void CropStation::get_crops(const int how_many_slots)
    {
        Sleep(500);
        turn_to_crop_plots();
        int slots_looted = 0;

        asa::entities::local_player->crouch();
        asa::entities::local_player->turn_down(25);
        empty(crop_plots_[0], slots_looted);
        if (slots_looted > how_many_slots) {
            return;
        }

        asa::entities::local_player->turn_up(25);
        empty(crop_plots_[1], slots_looted);
        if (slots_looted > how_many_slots) {
            return;
        }

        asa::entities::local_player->turn_up(25);
        empty(crop_plots_[2], slots_looted);
        if (slots_looted > how_many_slots) {
            return;
        }

        asa::entities::local_player->stand_up();
        asa::entities::local_player->turn_down(25);
        empty(crop_plots_[3], slots_looted);
        if (slots_looted > how_many_slots) {
            return;
        }

        asa::entities::local_player->turn_up(27);
        empty(crop_plots_[4], slots_looted);
        if (slots_looted > how_many_slots) {
            return;
        }

        asa::entities::local_player->turn_up(25);
        empty(crop_plots_[5], slots_looted);
        if (slots_looted > how_many_slots) {
        }
    }
}

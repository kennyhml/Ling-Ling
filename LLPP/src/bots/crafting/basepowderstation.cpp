#include "basepowderstation.h"
#include <asapp/core/state.h>
#include "embeds.h"
#include "asapp/entities/localplayer.h"

namespace llpp::bots::crafting
{
    BasePowderStation::BasePowderStation(const std::string& t_name,
                                         const std::chrono::minutes t_interval,
                                         const asa::items::Item& t_to_craft,
                                         const int t_amount_per_queue) :
        BedStation(t_name, t_interval), amount_to_queue_(t_amount_per_queue),
        to_craft_(t_to_craft), chem_bench_(name_ + "::CHEMBENCH", 100) {}

    void BasePowderStation::await_deposited()
    {
        asa::entities::local_player->get_inventory()->open();
        const auto& slot = asa::entities::local_player->get_inventory()->slots[1];

        if (!util::await([&slot]() { return slot.is_empty(); },
                         std::chrono::seconds(10))) {
            put_overproduce_back();
            set_completed();
            throw StationFullError(std::format("Station '{}' is full.", name_));
        }
        asa::entities::local_player->get_inventory()->close();
        asa::core::sleep_for(std::chrono::milliseconds(500));
    }

    void BasePowderStation::empty()
    {
        asa::entities::local_player->access(chem_bench_);

        // check if theres anything to even empty.
        if (!util::await([this]() -> bool { return chem_bench_.get_current_slots() > 5; },
                         std::chrono::seconds(1))) {
            chem_bench_.get_inventory()->close();
            asa::core::sleep_for(std::chrono::seconds(1));
            return;
        }

        chem_bench_.get_inventory()->transfer_all();
        chem_bench_.get_inventory()->close();

        asa::core::sleep_for(std::chrono::seconds(1));
        asa::entities::local_player->set_yaw(180);

        asa::entities::local_player->turn_down(20);
        asa::entities::local_player->crouch();

        for (int i = 0; i < 3; i++) {
            asa::core::sleep_for(300ms);
            asa::window::press(asa::settings::use);
            asa::core::sleep_for(300ms);

            if (i == 0) { asa::entities::local_player->stand_up(); }
            else if (i == 1) { asa::entities::local_player->turn_up(50); }
        }

        // get a screenshot of the dedis to post to discord alongside the error
        // if they are all capped.
        asa::entities::local_player->turn_down(30);
        asa::core::sleep_for(std::chrono::seconds(1));
        last_dedi_screenshot_ = asa::window::screenshot();

        await_deposited();
        asa::entities::local_player->set_yaw(0);
    }

    void BasePowderStation::requeue()
    {
        asa::entities::local_player->access(chem_bench_);
        was_still_crafting_ = chem_bench_.get_inventory()->is_crafting();
        chem_bench_.get_inventory()->craft(to_craft_, amount_to_queue_);
        asa::core::sleep_for(std::chrono::seconds(1));
    }

    void BasePowderStation::put_overproduce_back()
    {
        asa::entities::local_player->get_inventory()->close();
        asa::core::sleep_for(std::chrono::seconds(1));
        asa::entities::local_player->set_yaw(0);
        asa::entities::local_player->access(chem_bench_);
        asa::entities::local_player->get_inventory()->transfer_all();
        chem_bench_.get_inventory()->cancel_craft();
        chem_bench_.get_inventory()->close();
        asa::core::sleep_for(std::chrono::seconds(1));
    }
}

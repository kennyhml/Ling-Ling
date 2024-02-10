#include "arbstation.h"
#include <asapp/entities/localplayer.h>
#include <asapp/core/state.h>
#include "basepowderstation.h"
#include <asapp/items/items.h>
#include "embeds.h"

namespace llpp::bots::crafting
{
    ARBStation::ARBStation(std::string t_name, const std::chrono::minutes t_interval)
        : BedStation(std::move(t_name), t_interval), fabricator_(name_ + "::FAB", 70) {}


    core::StationResult ARBStation::complete()
    {
        if (!begin()) {
            return {this, false, get_time_taken<std::chrono::seconds>(), {}};
        }

        requeue();
        empty();

        set_completed();
        core::StationResult res(this, true, get_time_taken<std::chrono::seconds>(),
                                {{"Advanced Rifle Bullet", last_amount_taken_}});
        send_arb_crafted(res, was_still_crafting_);
        return res;
    }

    void ARBStation::empty()
    {
        // check if theres anything to even empty.
        if (!util::await([this]() -> bool { return fabricator_.get_current_slots() > 3; },
                         std::chrono::seconds(1))) {
            fabricator_.get_inventory()->close();
            asa::core::sleep_for(std::chrono::seconds(1));
            return;
        }
        asa::core::sleep_for(std::chrono::seconds(1));
        last_amount_taken_ = fabricator_.get_current_slots() * 100;

        fabricator_.get_inventory()->transfer_all();
        fabricator_.get_inventory()->close();
        asa::core::sleep_for(std::chrono::seconds(1));

        asa::entities::local_player->set_yaw(180);
        asa::entities::local_player->crouch();
        asa::entities::local_player->turn_down(20);

        for (int i = 0; i < 3; i++) {
            asa::core::sleep_for(std::chrono::milliseconds(500));
            asa::window::press(asa::settings::use);

            if (i == 0) { asa::entities::local_player->stand_up(); }
            else if (i == 1) { asa::entities::local_player->turn_up(50); }
        }

        // get a screenshot of the dedis to post to discord alongside the error
        // if they are all capped.
        asa::core::sleep_for(std::chrono::seconds(1));
        asa::entities::local_player->turn_down(30);

        last_dedi_screenshot_ = asa::window::screenshot();

        await_deposited();
        asa::entities::local_player->set_yaw(0);
    }

    void ARBStation::await_deposited()
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

    void ARBStation::put_overproduce_back()
    {
        asa::entities::local_player->get_inventory()->close();
        asa::core::sleep_for(std::chrono::seconds(1));
        asa::entities::local_player->set_yaw(0);
        asa::entities::local_player->access(fabricator_);
        asa::entities::local_player->get_inventory()->transfer_all();
        fabricator_.get_inventory()->cancel_craft();
        fabricator_.get_inventory()->close();
        asa::core::sleep_for(std::chrono::seconds(1));
    }

    void ARBStation::requeue()
    {
        asa::entities::local_player->access(fabricator_);
        was_still_crafting_ = fabricator_.get_inventory()->is_crafting();
        fabricator_.get_inventory()->craft(*asa::items::ammo::advanced_rifle_bullet,
                                           1000);
        asa::core::sleep_for(std::chrono::seconds(1));
    }
}

#include "grindstation.h"

#include <iostream>
#include <asapp/core/state.h>
#include <asapp/items/items.h>
#include "embeds.h"
#include "asapp/entities/localplayer.h"
#include <asapp/interfaces/tribemanager.h>

namespace llpp::bots::crafting
{
    GrindStation::GrindStation(std::string name, const std::chrono::minutes interval) :
        BedStation(std::move(name), interval), grinder_(name_ + "::GRINDER", 120),
        paste_dedi_(name_ + "::PASTE") {}

    core::StationResult GrindStation::complete()
    {
        if (!begin()) {
            return {this, false, get_time_taken<std::chrono::seconds>(), {}};
        }
        bool was_emptied = false;

        if (state_ == GRINDING) {
            empty_grinder();
            deposit_items();
            state_ = WAITING;
            was_emptied = true;
        }

        take_paste();
        put_paste_and_grind();

        flint_grind_start_ = std::chrono::system_clock::now();
        state_ = GRINDING;
        set_completed();

        core::StationResult res(this, true, get_time_taken<std::chrono::seconds>(), {});
        send_paste_grinded(res, was_emptied);
        return res;
    }

    void GrindStation::take_paste()
    {
        using asa::interfaces::DedicatedStorageInfo;

        asa::entities::local_player->turn_left(90);
        asa::entities::local_player->turn_down(20);
        asa::entities::local_player->crouch();
        asa::entities::local_player->access(paste_dedi_);

        if (!paste_dedi_.get_info()->withdraw(DedicatedStorageInfo::THIRD)) {
            // this means the station is out of paste, signal that it must be disabled until
            // the dedi is refilled and re-enabled.
        }

        paste_dedi_.get_inventory()->close();
        asa::core::sleep_for(std::chrono::seconds(1));
    }

    void GrindStation::put_paste_and_grind()
    {
        std::cout << "[+] Putting paste away..\n";
        asa::entities::local_player->stand_up();
        asa::entities::local_player->turn_left(90);
        asa::entities::local_player->access(grinder_);
        asa::core::sleep_for(std::chrono::seconds(1));

        asa::entities::local_player->get_inventory()->transfer_all();
        util::await([]() {
            return !asa::entities::local_player->get_inventory()->slots[1].has(
                *asa::items::resources::achatina_paste);
        }, std::chrono::seconds(5));

        for (int i = 0; i < 5; i++) {
            asa::window::post_mouse_press_at({963, 832}, asa::controls::LEFT);
            asa::core::sleep_for(std::chrono::milliseconds(300));
        }

        grinder_.get_inventory()->craft(0, 1000);
        grinder_.get_inventory()->transfer_all(*asa::items::resources::chitin);
        asa::core::sleep_for(std::chrono::seconds(1));
        grinder_.get_inventory()->close();
        asa::core::sleep_for(std::chrono::seconds(1));
    }

    void GrindStation::empty_grinder()
    {
        asa::entities::local_player->turn_right(180);
        asa::entities::local_player->access(grinder_);
        asa::core::sleep_for(std::chrono::milliseconds(300));
        grinder_.get_inventory()->transfer_all();
        asa::core::sleep_for(std::chrono::milliseconds(300));
        grinder_.get_inventory()->close();
        asa::core::sleep_for(std::chrono::seconds(1));

        asa::entities::local_player->turn_left(180);
    }

    void GrindStation::deposit_items()
    {
        asa::entities::local_player->crouch();
        asa::entities::local_player->turn_down(20, std::chrono::milliseconds(600));

        asa::entities::local_player->turn_left(20, std::chrono::milliseconds(600));
        asa::controls::press(asa::settings::use);

        asa::entities::local_player->turn_right(40, std::chrono::milliseconds(600));
        asa::controls::press(asa::settings::use);

        asa::entities::local_player->stand_up();
        asa::entities::local_player->turn_up(20, std::chrono::milliseconds(600));
        asa::controls::press(asa::settings::use);

        asa::entities::local_player->turn_left(40, std::chrono::milliseconds(600));
        asa::controls::press(asa::settings::use);

        asa::entities::local_player->turn_up(20, std::chrono::milliseconds(600));
        asa::controls::press(asa::settings::use);

        asa::entities::local_player->turn_right(40, std::chrono::milliseconds(600));
        asa::controls::press(asa::settings::use);

        asa::entities::local_player->set_yaw(0);
        asa::entities::local_player->set_pitch(0);
    }

    bool GrindStation::flint_finished() const
    {
        return util::timedout(flint_grind_start_, std::chrono::seconds(360));
    }
}

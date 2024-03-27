#include <iostream>
#include "pastestation.h"
#include <asapp/core/state.h>
#include <asapp/entities/exceptions.h>
#include <asapp/structures/exceptions.h>
#include <asapp/entities/localplayer.h>
#include <asapp/items/items.h>
#include "embeds.h"
#include "../../common/util.h"
#include "../../config/config.h"

namespace llpp::bots::paste
{
    PasteStation::PasteStation(std::string t_name,
                               std::chrono::system_clock::time_point t_last_completed,
                               const std::chrono::minutes t_interval) : BedStation(
        std::move(t_name), t_last_completed, t_interval), dedi_(name_ + "::DEDI") {};

    core::StationResult PasteStation::complete()
    {
        if (!begin()) {
            return {this, false, get_time_taken<std::chrono::seconds>(), {}};
        }

        empty_all();
        int obtained = deposit_paste();
        set_completed();

        core::StationResult res(this, true, get_time_taken<std::chrono::seconds>(),
                                {{"Achatina Paste", obtained}});
        send_paste_collected(res);
        return res;
    }

    bool PasteStation::empty(asa::entities::DinoEntity& achatina)
    {
        try { asa::entities::local_player->access(achatina); }
        catch (const asa::entities::EntityNotAccessed& e) {
            send_achatina_not_accessible(name_, e.get_entity()->get_name());
            return true;
        }

        const auto& slot = achatina.get_inventory()->slots[0];
        const bool has_paste = slot.has(*asa::items::resources::achatina_paste);
        if (!has_paste) { std::cerr << "[!] Achatina has no paste in it\n"; }
        else {
            std::cout << "Achatina has paste, taking it...\n";
            achatina.get_inventory()->take_slot(0);
        }
        achatina.get_inventory()->close();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return has_paste;
    }

    void PasteStation::empty_all()
    {
        std::cout << "[+] Emptying the achatinas..." << std::endl;
        empty(achatinas[4]);
        asa::entities::local_player->turn_left(45);
        empty(achatinas[3]);
        asa::entities::local_player->turn_right(90);
        empty(achatinas[5]);

        std::this_thread::sleep_for(std::chrono::seconds(1));
        asa::entities::local_player->crouch();
        asa::entities::local_player->turn_down(12, std::chrono::milliseconds(300));

        empty(achatinas[2]);
        asa::entities::local_player->turn_left(45);
        empty(achatinas[1]);

        asa::entities::local_player->turn_left(45);
        empty(achatinas[0]);
    }

    int PasteStation::deposit_paste()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        asa::entities::local_player->set_yaw(180);
        asa::core::sleep_for(std::chrono::seconds(1));

        if (!turn_until_deposit()) {
            send_paste_not_deposited(name_);
            return 0;
        }

        int amount = 0;
        try {
            if (config::bots::paste::ocr_amount.get()) {
                asa::entities::local_player->deposit_into_dedi(
                    *asa::items::resources::achatina_paste, &amount);
            }
            else {
                asa::entities::local_player->deposit_into_dedi(
                    *asa::items::resources::achatina_paste, nullptr);
                return util::get_elapsed<std::chrono::minutes>(last_completed_).count() *
                    6;
            }
        }
        catch (const asa::structures::StructureError&) {
            std::cerr << "[!] Depositing failed" << std::endl;
            send_paste_not_deposited(name_);
        }
        return amount;
    }

    bool PasteStation::turn_until_deposit() const
    {
        if (dedi_.can_deposit()) { return true; }

        for (int i = 0; i < 3; i++) {
            asa::entities::local_player->turn_left(45, std::chrono::milliseconds(900));
            if (dedi_.can_deposit()) { return true; }
        }
        // return to where we started turning left and go right instead.
        asa::entities::local_player->set_yaw(180);
        asa::core::sleep_for(std::chrono::seconds(1));

        for (int i = 0; i < 3; i++) {
            asa::entities::local_player->turn_right(45, std::chrono::milliseconds(500));
            if (dedi_.can_deposit()) { return true; }
        }
        return false;
    }
}

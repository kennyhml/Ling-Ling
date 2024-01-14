#include <iostream>
#include "pastestation.h"
#include <asapp/entities/exceptions.h>
#include <asapp/structures/exceptions.h>
#include <asapp/entities/localplayer.h>
#include <asapp/items/items.h>

#include "embeds.h"
#include "../../common/util.h"
#include "../../config/config.h"

namespace llpp::bots::paste
{
    PasteStation::PasteStation(std::string name, std::chrono::minutes interval) :
        BaseStation(name, interval), bed(asa::structures::SimpleBed(name)) {};

    core::StationResult PasteStation::complete()
    {
        auto start = std::chrono::system_clock::now();

        asa::entities::local_player->fast_travel_to(bed);
        empty_all();
        int pasteObtained = deposit_paste();
        set_completed();

        auto duration = util::get_elapsed<std::chrono::seconds>(start);
        core::StationResult resultData(this, true, duration,
                                       {{"Achatina Paste", pasteObtained}});

        send_paste_collected(resultData);
        return resultData;
    }

    bool PasteStation::empty(asa::entities::DinoEnt& achatina)
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
        asa::entities::local_player->turn_left(135, std::chrono::milliseconds(300));

        int amount = 0;
        try {
            if (config::bots::paste::ocr_amount.get()) {
                asa::entities::local_player->deposit_into_dedi(
                    *asa::items::resources::achatina_paste, &amount);
            }
            else {
                asa::entities::local_player->deposit_into_dedi(
                    *asa::items::resources::achatina_paste, nullptr);
            }
        }
        catch (const asa::structures::StructureError&) {
            std::cerr << "[!] Depositing & OCR failed, trying without...\n";
            asa::entities::local_player->deposit_into_dedi(
                *asa::items::resources::achatina_paste, nullptr);
        }
        return amount;
    }
}

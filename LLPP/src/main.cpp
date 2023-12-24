#include <asapp/core/exceptions.h>
#include <asapp/entities/localplayer.h>
#include "bots/drops/cratemanager.h"
#include "bots/paste/pastemanager.h"
#include "bots/suicide/suicidestation.h"
#include "core/discord.h"
#include "core/recovery.h"
#include <iostream>
#include <asapp/core/init.h>
#include <dpp/dpp.h>

#include <fstream>
#include <nlohmann/json.hpp>
#include "bots/drops/embeds.h"
#include "bots/kitchen/cropstation.h"
#include "core/data/database.h"

#include <asapp/items/items.h>
#include <opencv2/highgui.hpp>
#include "common/util.h"

using json = nlohmann::json;

int main()
{
    if (!asa::core::init(std::filesystem::path("src/config.json"))) {
        std::cerr << "[!] Failed to initialize ASAPP\n";
        return 0;
    }

    llpp::core::data::init("test.db");
    std::ifstream f("src/config.json");
    json data = json::parse(f);
    f.close();
    llpp::core::discord::init(data["bot"]);
    llpp::core::discord::bot->start(dpp::st_return);


    asa::window::get_handle(60, true);
    asa::window::set_foreground();


    auto achatina = asa::entities::DinoEnt("Achatina");
    auto paste = llpp::bots::paste::PasteManager("PASTE", 15, std::chrono::minutes(50));

    paste.run();
    return 0;

    auto station = llpp::bots::kitchen::CropStation("ROCKARROT01", true,
                                                    llpp::bots::kitchen::CropStation::ROCKARROT,
                                                    std::chrono::minutes(30));


    using quality = asa::structures::CaveLootCrate::Quality;

    auto suicide_station = llpp::bots::suicide::SuicideStation(
        "SUICIDE DEATH", "SUICIDE RESPAWN");

    auto swamp = llpp::bots::drops::CrateManager(
        "SWAMP", {
            {quality::RED, quality::RED},
            {quality::YELLOW, quality::YELLOW, quality::ANY}, {quality::BLUE}
        }, std::chrono::minutes(5), &suicide_station);

    auto skylord = llpp::bots::drops::CrateManager("SKYLORD", {
                                                       {
                                                           quality::YELLOW | quality::RED,
                                                           quality::YELLOW | quality::RED,
                                                           quality::YELLOW | quality::RED
                                                       },
                                                   }, std::chrono::minutes(5));


    asa::window::get_handle();
    try { asa::entities::local_player->get_inventory()->drop_all(); }
    catch (std::exception& e) { std::cout << e.what() << "\n"; }

    return 0;


    auto vault = asa::structures::Container("Vault", 350);
    auto slots = vault.inventory->slots;

    auto start = std::chrono::system_clock::now();
    for (size_t i = 0; i < slots.size(); i++) {
        auto item = slots[i].get_item();
        if (item.get()) { std::cout << i << ": " << item.get()->info() << "\n"; }
        else { std::cout << i << ": Undetermined\n"; }
    }

    std::cout << "Time taken: " << llpp::util::get_elapsed<
        std::chrono::milliseconds>(start) << "\n";
    return 0;

    while (true) {
        try {
            if (swamp.run()) { continue; }
            if (paste.run()) { continue; }
            if (skylord.run()) { continue; }

            std::cout << "No task ready....\n";
        }
        catch (asa::core::ShooterGameError& e) {
            llpp::core::inform_crash_detected(e);
            llpp::core::reconnect_to_server();
        } catch (const std::exception& e) {
            llpp::core::discord::inform_fatal_error(e, "???");
            break;
        }
    }
    return 0;
}

#include <asapp/core/exceptions.h>
#include <asapp/entities/localplayer.h>
#include "bots/drops/cratemanager.h"
#include "bots/paste/pastemanager.h"
#include "bots/suicide/suicidestation.h"
#include "core/discord.h"
#include "core/recovery.h"

#include <asapp/core/init.h>
#include <dpp/dpp.h>

#include <fstream>
#include <nlohmann/json.hpp>

#include "bots/drops/embeds.h"
#include "bots/kitchen/cropstation.h"
#include "core/data/database.h"

using json = nlohmann::json;

int main()
{
    llpp::core::data::init("test.db");

    if (!asa::core::init(std::filesystem::path("src/config.json"))) {
        std::cerr << "[!] Failed to initialize ASAPP\n";
        return 0;
    }
    asa::window::get_handle(60, true);
    asa::window::set_foreground();

    auto station = llpp::bots::kitchen::CropStation("ROCKARROT01", true,
                                                    llpp::bots::kitchen::CropStation::ROCKARROT,
                                                    std::chrono::minutes(30));


    std::ifstream f("src/config.json");
    json data = json::parse(f);
    f.close();

    llpp::core::discord::init(data["bot"]);

    llpp::core::discord::bot->start(dpp::st_return);
    station.complete();

    Sleep(5000);
    return 0;

    using quality = asa::structures::CaveLootCrate::Quality;

    auto suicide_station = llpp::bots::suicide::SuicideStation(
        "SUICIDE DEATH", "SUICIDE RESPAWN");

    auto swamp = llpp::bots::drops::CrateManager("SWAMP",
                                                 {
                                                     {quality::RED, quality::RED},
                                                     {
                                                         quality::YELLOW, quality::YELLOW,
                                                         quality::ANY
                                                     },
                                                     {quality::BLUE}
                                                 },
                                                 std::chrono::minutes(5),
                                                 &suicide_station);

    auto paste = llpp::bots::paste::PasteManager(
        "PASTE", 6, std::chrono::minutes(50));
    auto skylord = llpp::bots::drops::CrateManager("SKYLORD",
                                                   {
                                                       {
                                                           quality::YELLOW | quality::RED,
                                                           quality::YELLOW | quality::RED,
                                                           quality::YELLOW | quality::RED
                                                       },
                                                   },
                                                   std::chrono::minutes(5));


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
        }
        catch (const std::exception& e) {
            llpp::core::discord::inform_fatal_error(e, "???");
            break;
        }
    }
    return 0;
}

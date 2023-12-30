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

#include <nlohmann/json.hpp>
#include "bots/drops/embeds.h"
#include "bots/kitchen/cropstation.h"
#include "core/data/database.h"
#include <opencv2/highgui.hpp>

#include "../external/imgui/imgui_internal.h"
#include "common/util.h"
#include "gui/gui.h"

using json = nlohmann::json;


int WINAPI WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prev_instance,
                   _In_ PSTR cmd_line, _In_ int cmd_show)
{
    llpp::gui::create_window(L"Ling Ling++", L"Meow");
    llpp::gui::create_device();
    llpp::gui::create_imgui();

    if (!AllocConsole()) { return false; }
    FILE* pFile;
    freopen_s(&pFile, "CONOUT$", "w", stdout) != 0;

    while (llpp::gui::exit) {
        llpp::gui::begin_render();

        // Render ImGui
        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize({
            ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y
        });

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(620, 420));
        llpp::gui::render();
        ImGui::ShowStyleEditor();
        ImGui::PopStyleVar(2);
        llpp::gui::end_render();
    }
    llpp::gui::destroy_imgui();
    llpp::gui::destroy_device();
    llpp::gui::destroy_window();

    return EXIT_SUCCESS;

    if (!asa::core::init(std::filesystem::path("src/config.json"))) {
        std::cerr << "[!] Failed to initialize ASAPP\n";
        return 0;
    }

    llpp::core::data::init("test.db");
    std::ifstream f("src/config.json");
    json data = json::parse(f);
    f.close();

    llpp::core::discord::init(data["bot"]);
    asa::window::get_handle(60, true);
    asa::window::set_foreground();

    auto paste = llpp::bots::paste::PasteManager("PASTE", 16, std::chrono::minutes(50));
    auto suicide_station = llpp::bots::suicide::SuicideStation(
        "SUICIDE DEATH", "SUICIDE RESPAWN");

    using quality = asa::structures::CaveLootCrate::Quality;

    auto huw = llpp::bots::drops::CrateManager(
        {"HUW", true, std::chrono::minutes(5), std::chrono::seconds(8)},
        {{quality::ANY}, {quality::ANY}, {quality::ANY}}, &suicide_station);


    auto swamp = llpp::bots::drops::CrateManager(
        {"SWAMP", false, std::chrono::minutes(5), std::chrono::seconds(1)}, {
            {quality::RED, quality::RED},
            {quality::YELLOW, quality::YELLOW, quality::ANY}, {quality::BLUE}
        }, &suicide_station);

    auto skylord = llpp::bots::drops::CrateManager(
        {"SKYLORD", false, std::chrono::minutes(5), std::chrono::seconds(0)}, {
            {
                quality::YELLOW | quality::RED, quality::YELLOW | quality::RED,
                quality::YELLOW | quality::RED
            },
        });

    llpp::core::discord::bot->start(dpp::st_return);

    while (true) {
        try {
            if (swamp.run()) { continue; }
            if (skylord.run()) { continue; }
            if (huw.run()) { continue; }
            if (paste.run()) { continue; }
            std::cout << "No task ready....\n";
        }
        catch (asa::core::ShooterGameError& e) {
            llpp::core::inform_crash_detected(e);
            llpp::core::recover();
        }catch (const std::exception& e) {
            llpp::core::discord::inform_fatal_error(e, "???");
            break;
        }
    }
    return 0;
}

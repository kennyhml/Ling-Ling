#include <iostream>
#include <asapp/core/exceptions.h>
#include <asapp/core/init.h>
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include <asapp/items/items.h>
#include <asapp/interfaces/hud.h>

#include "bots/drops/cratemanager.h"
#include "bots/kitchen/cropmanager.h"
#include "bots/kitchen/sapmanager.h"
#include "bots/paste/pastemanager.h"
#include "bots/suicide/suicidestation.h"
#include "common/util.h"
#include "gui/gui.h"
#include "config/config.h"
#include "core/discord.h"
#include "core/recovery.h"


static bool running = false;

void llpp_main()
{
    using namespace llpp::config::bots;

    if (!asa::core::init(llpp::config::general::ark::root_dir.get(),
                         llpp::config::general::bot::assets_dir.get(),
                         llpp::config::general::bot::itemdata.get(),
                         llpp::config::general::bot::tessdata_dir.get())) {
        std::cerr << "[!] Failed to init asapp\n";
        return;
    }

    asa::window::get_handle();

    llpp::core::discord::init();
    auto paste = llpp::bots::paste::PasteManager(paste::prefix.get(),
                                                 paste::num_stations.get(),
                                                 std::chrono::minutes(
                                                     paste::interval.get()));

    std::vector<llpp::bots::drops::CrateManager*> crate_managers{};

    for (auto& [key, config] : drops::configs) {
        crate_managers.push_back(new llpp::bots::drops::CrateManager(*config.get_ptr()));
    }

    auto crops = llpp::bots::kitchen::CropManager();
    auto sap = llpp::bots::kitchen::SapManager(sap::prefix.get(), sap::num_stations.get(),
                                               std::chrono::minutes(sap::interval.get()));
    auto suicide =
        llpp::bots::suicide::SuicideStation("SUICIDE DEATH", "SUICIDE RESPAWN");

    llpp::core::discord::bot->start(dpp::st_return);
    llpp::core::discord::inform_started();

    while (running) {
        try {
            asa::entities::local_player->get_inventory()->open();
            Sleep(2000);
            auto& info = asa::entities::local_player->get_inventory()->info;
            if (info.get_health_level() < 0.75f || info.get_water_level() < 0.75f || info.
                get_food_level() < 0.75f) {
                asa::entities::local_player->get_inventory()->close();
                suicide.complete();
            }
            else { asa::entities::local_player->get_inventory()->close(); }

            if (std::any_of(crate_managers.begin(), crate_managers.end(),
                            [](llpp::bots::drops::CrateManager* m) {
                                return m->run();
                            })) {
                Sleep(3000);
                continue;
            }
            if (paste.run()) { continue; }
            if (crops.run()) {}
            if (sap.run()) {}
        }
        catch (asa::core::ShooterGameError& e) {
            llpp::core::inform_crash_detected(e);
            llpp::core::recover();
        } catch (const std::exception& e) {
            llpp::core::discord::inform_fatal_error(e, "?");
            running = false;
        }
    }

    llpp::core::discord::bot->shutdown();
}

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

        if ((GetAsyncKeyState(VK_F1) & 0x1) && !running) {
            running = true;
            std::thread(llpp_main).detach();
        }

        if ((GetAsyncKeyState(VK_F3) & 0x1) && running) { running = false; }
    }

    llpp::gui::destroy_imgui();
    llpp::gui::destroy_device();
    llpp::gui::destroy_window();
    return EXIT_SUCCESS;
}

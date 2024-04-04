#include <iostream>
#include <asapp/core/exceptions.h>
#include <asapp/core/init.h>
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/console.h>
#include <asapp/interfaces/tribemanager.h>
#include "bots/farm/swingbot/commands.h"
#include "auth/auth.h"
#include "gui/gui.h"
#include "config/config.h"
#include "core/recovery.h"
#include "core/taskmanager.h"
#include <curl/curl.h>
#include "common/util.h"
#include "discord/bot.h"
#include "discord/embeds.h"


static bool running = false;
static bool paused = false;

class TerminatedError : public std::exception
{
public:
    using exception::exception;
};

void check_terminated() { while(running && paused) { asa::core::sleep_for(10ms); } if (!running) { throw TerminatedError(); } }

#include <opencv2/core/utils/logger.hpp>


void inform_started()
{
    llpp::discord::get_bot()->message_create(llpp::discord::create_started_message());
}

void inform_crashed(const std::exception& why, const std::string& task)
{
    llpp::discord::get_bot()->message_create(
        llpp::discord::create_fatal_error_message(why, task));
}

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

    auto taskmanager = llpp::core::TaskManager();
    try {
        asa::window::get_handle();
        asa::window::set_foreground();
        llpp::discord::init();
        taskmanager.collect_tasks();
        llpp::bots::farm::register_commands();
    } catch (const llpp::config::BadConfigurationError& e) {
        std::cerr << "[!] Configuration error " << e.what() << std::endl;
        return;
    } catch (const std::exception& e) { std::cerr << e.what() << "\n"; }
    llpp::discord::get_bot()->start(dpp::st_return);
    inform_started();

    try {
        asa::interfaces::console->execute(llpp::config::general::bot::commands.get());
        asa::entities::local_player->reset_state();
    } catch (const TerminatedError&) {}

    while (running) {
        try { taskmanager.execute_next(); } catch (asa::core::ShooterGameError& e) {
            llpp::core::inform_crash_detected(e);
            llpp::core::recover();
        } catch (const TerminatedError&) { break; } catch (const std::exception& e) {
            inform_crashed(e, taskmanager.get_previous_task()->get_name());
            running = false;
        }
    }

    llpp::discord::get_bot()->shutdown();
    std::cout << "[+] Ling Ling++ has terminated!" << std::endl;
}

int WINAPI WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prev_instance,
                   _In_ PSTR cmd_line, _In_ int cmd_show)
{
    // Then the logging level can be set with the following function
    cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

    if (!AllocConsole()) { return false; }

    FILE* pFile;
    if (freopen_s(&pFile, "CONIN$", "r", stdin) != 0) {
        // Handle error, if any
        return false;
    }

    if (freopen_s(&pFile, "CONOUT$", "w", stdout) != 0) {
        // Handle error, if any
        return false;
    }

    if (freopen_s(&pFile, "CONOUT$", "w", stderr) != 0) {
        // Handle error, if any
        return false;
    }

    llpp::auth::login();

    llpp::gui::create_window(L"Ling Ling++", L"Meow");
    llpp::gui::create_device();
    llpp::gui::create_imgui();

    asa::core::register_state_callback(check_terminated);

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
            paused = false;
            std::thread(llpp_main).detach();
        }

        if ((GetAsyncKeyState(VK_F5) & 0x1) && running) {
            if(!paused) {
                std::cout << "[+] Pause signal sent, please give it a few seconds...\n";
                paused = true;
            } else {
                std::cout << "[+] Unpause signal sent, please give it a few seconds...\n";
                paused = false;
            }
        }

        if ((GetAsyncKeyState(VK_F3) & 0x1) && running) {
            std::cout << "[+] Termination signal sent, please give it a few seconds...\n";
            running = false;
        }
    }

    llpp::gui::destroy_imgui();
    llpp::gui::destroy_device();
    llpp::gui::destroy_window();
    return EXIT_SUCCESS;
}

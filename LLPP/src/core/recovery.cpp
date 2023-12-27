#include "recovery.h"
#include "../common/util.h"
#include "discord.h"
#include <asapp/entities/localplayer.h>
#include <asapp/game/window.h>
#include <asapp/interfaces/mainmenu.h>
#include <asapp/interfaces/modeselect.h>
#include <asapp/interfaces/serverselect.h>
#include <asapp/interfaces/spawnmap.h>

namespace llpp::core
{
    void recover()
    {
        const auto start = std::chrono::system_clock::now();
        asa::core::set_crash_aware(true);
        std::cout << "[+] Recovery sequence initiated, diagnosing cause of crash...\n";

        bool need_restart = false;
        bool need_reconnect = false;

        if (asa::window::has_crashed_popup()) {
            std::cout << "\t[-] The game has crashed...\n";
            need_restart = true;
            need_reconnect = true;
        }
        else if (asa::interfaces::main_menu->is_open()) {
            std::cout << "\t[-] Kicked to main menu...\n";
            need_reconnect = true;
        }

        // inform_recovery_initiated(need_restart, need_reconnect);
        if (need_restart) { restart_game(); }
        reconnect_to_server();
        // inform_recovery_successful(util::get_elapsed<std::chrono::seconds>(start));
        asa::core::set_crash_aware(false);

        if (asa::interfaces::spawn_map->is_open()) {
            asa::interfaces::spawn_map->spawn_at("RESET BED");
            std::this_thread::sleep_for(std::chrono::seconds(15));
        }
    }

    void reconnect_to_server()
    {
        const std::string server_name = "NA-PVP-TheIsland2207";

        asa::interfaces::main_menu->accept_popup();
        asa::interfaces::main_menu->start();
        Sleep(1000);
        asa::interfaces::mode_select->join_game();
        Sleep(1000);
        asa::interfaces::server_select->join_server(server_name);

        auto start = std::chrono::system_clock::now();

        while (true) {
            if (asa::entities::local_player->is_alive() || asa::interfaces::spawn_map->
                is_open()) { break; }

            if (asa::interfaces::main_menu->is_open()) {
                std::cerr << "[!] Joining failed, trying again\n";
                return reconnect_to_server();
            }
        }

        Sleep(5000);
        std::cout << "[+] Reconnected successfully.\n";
    }

    void restart_game()
    {
        exit_game();
        Sleep(5000);
        if (system("start steam://rungameid/2399830") == -1) {
            throw std::runtime_error("Failed to restart the game");
        }
        asa::window::get_handle(60, true);

        while (!asa::interfaces::main_menu->is_open()) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }

        std::cout << "[+] Game restarted.\n";
    }

    void exit_game()
    {
        DWORD pid;

        GetWindowThreadProcessId(asa::window::hWnd, &pid);
        const HANDLE hprocess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        if (!hprocess) { throw std::runtime_error("Process handle not acquired"); }
        TerminateProcess(hprocess, 0);
        CloseHandle(hprocess);
    }

    void inform_crash_detected(asa::core::ShooterGameError& e)
    {
        if (!discord::bot) {
            std::cerr << "[!] Skipped posting paste success, no discord bot\n";
            return;
        }

        auto embed = dpp::embed();
        embed.set_color(0xB82E88).set_title("CRITICAL: The game or server has crashed.").
              set_description(e.what()).set_thumbnail(
                  "https://static.wikia.nocookie.net/" "arksurvivalevolved_gamepedia/"
                  "images/5/53/Mission_Area.png/revision/" "latest?cb=20200314145130").
              add_field("Assistance required:", "False").
              set_image("attachment://image.png").set_footer(
                  dpp::embed_footer("Recovery should follow automatically."));

        const auto file_data = util::mat_to_strbuffer(asa::window::screenshot());
        dpp::message message = dpp::message(dpp::snowflake(discord::info_channel_id),
                                            "<@&1181159721433051136>").
            set_allowed_mentions(false, true, false, false, {}, {});
        message.add_file("image.png", file_data, "image/png ").add_embed(embed);

        discord::bot->message_create(message);
    }

    void inform_recovery_initiated(const bool restart, const bool reconnect)
    {
        if (!discord::bot) {
            std::cerr << "[!] Skipped posting paste success, no discord bot\n";
            return;
        }
        auto embed = dpp::embed();
        embed.set_color(0x7F5D44).set_title("Recovery sequence was initiated!").
              set_description("Recovery attempt imminent. Required steps:").set_thumbnail(
                  "https://static.wikia.nocookie.net/" "arksurvivalevolved_gamepedia/"
                  "images/2/24/Crafting_Light.png/revision/" "latest?cb=20181217123945").
              add_field("Restart required:", restart ? "Yes" : "No", true).add_field(
                  "Reconnect required:", reconnect ? "Yes" : "No", true);

        if (reconnect) { embed.add_field("Reconnecting to:", "9236", true); }

        const auto message = dpp::message(discord::info_channel_id, embed);
        discord::bot->message_create(message);
    }

    void inform_recovery_successful(const std::chrono::seconds time_taken)
    {
        if (!discord::bot) {
            std::cerr << "[!] Skipped posting paste success, no discord bot\n";
            return;
        }
        auto embed = dpp::embed();
        embed.set_color(dpp::colors::green).set_title("Recovery sequence was successful!")
             .set_description("Ling Ling++ has successfully recovered itself.").
             set_thumbnail(
                 "https://static.wikia.nocookie.net/" "arksurvivalevolved_gamepedia/"
                 "images/b/b5/Imprinted.png/revision/latest?cb=20181217131908").add_field(
                 "Time taken:", std::format("{} seconds", time_taken.count())).set_image(
                 "attachment://image.png");


        const auto file_data = util::mat_to_strbuffer(asa::window::screenshot());
        auto message = dpp::message(discord::info_channel_id, embed);
        message.add_file("image.png", file_data, "image/png ");

        discord::bot->message_create(message);
    }
}

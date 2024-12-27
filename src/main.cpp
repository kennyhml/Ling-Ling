#include <iostream>
#include <windows.h>
#include <opencv2/core/utils/logger.hpp>

#include "asa/core/logging.h"
#include "asa/core/state.h"
#include "asa/entities/localplayer.h"
#include "asa/ui/console.h"

int WINAPI WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prev_instance,
                   _In_ PSTR cmd_line, _In_ int cmd_show)
{
    setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

    if (!AllocConsole()) { return false; }

    FILE* pFile;
    if (freopen_s(&pFile, "CONIN$", "r", stdin) != 0) {
        return false;
    }
    if (freopen_s(&pFile, "CONOUT$", "w", stdout) != 0) {
        return false;
    }
    if (freopen_s(&pFile, "CONOUT$", "w", stderr) != 0) {
        return false;
    }


    asa::load_game_settings("F:/SteamLibrary/steamapps/common/ARK Survival Ascended");
    asa::window::get_handle();
    asa::window::set_focus();


    auto con = asa::console();

    while (true) {
        asa::checked_sleep(1s);
        con.execute("ccc");
    }
    return 1;

    asa::get_local_player()->get_inventory()->open()
                           .switch_to(asa::local_inventory::INVENTORY)
                           .auto_stack()
                           .drop_all("stone")
                           .drop_all("wood")
                           .drop_all("flint")
                           .close();

    asa::window::get_handle();
    asa::window::set_focus();


    return EXIT_SUCCESS;
}

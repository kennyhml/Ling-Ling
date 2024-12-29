#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "asa/core/state.h"
#include "asa/ui/console.h"
#include <opencv2/core/utils/logger.hpp>
#include <dpp/dpp.h>

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

    return EXIT_SUCCESS;
}

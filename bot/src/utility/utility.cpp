#include "utility.h"
#include <fstream>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace lingling::utility
{
    void dump(const nlohmann::ordered_json& data, const std::filesystem::path& file)
    {
        std::ofstream write_file(file.c_str());
        write_file << std::setw(4) << data;
        write_file.close();
    }

    nlohmann::ordered_json read(const std::filesystem::path& file)
    {
        std::ifstream read_file(file.c_str());
        auto data = nlohmann::ordered_json::parse(read_file);
        read_file.close();
        return data;
    }

    void enable_virtual_terminal_processing()
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;

        if (hConsole == INVALID_HANDLE_VALUE) return;

        if (GetConsoleMode(hConsole, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hConsole, dwMode);
        }
    }
}

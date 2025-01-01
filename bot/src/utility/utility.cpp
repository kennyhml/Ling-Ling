#include "utility.h"
#include <fstream>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace lingling::utility
{
    std::vector<dpp::message> extract_messages_sorted(const dpp::message_map& map)
    {
        std::vector<dpp::message> ret;
        ret.reserve(map.size());
        std::ranges::transform(map, std::back_inserter(ret), [](const auto& p) {
            return p.second;
        });

        // Sort by creation time in ascending order (older messages first)
        std::ranges::sort(ret, [](const auto& m1, const auto& m2) {
            return m1.get_creation_time() < m2.get_creation_time();
        });

        return ret;
    }

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

    std::string make_counter(const std::chrono::system_clock::duration duration)
    {
        using namespace std::chrono;
        std::ostringstream oss;

        const auto d = duration_cast<days>(duration);
        const auto h = duration_cast<hours>(duration % 24h);
        const auto m = duration_cast<minutes>(duration % 60min);
        const auto s = duration_cast<seconds>(duration % 60s);

        if (d.count() > 0) {
            oss << std::setw(2) << std::setfill('0') << d.count() << ":";
        }
        if (h.count() > 0 || d.count() > 0) {
            oss << std::setw(2) << std::setfill('0') << h.count() << ":";
        }
        if (m.count() > 0 || h.count() > 0 || d.count() > 0) {
            oss << std::setw(2) << std::setfill('0') << m.count() << ":";
        }
        oss << std::setw(2) << std::setfill('0') << s.count();
        return oss.str();
    }
}

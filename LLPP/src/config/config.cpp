#include "config.h"
#include <fstream>
#include "required.h"
#include "../bots/drops/cratemanager.h"

namespace llpp::config
{
    namespace
    {
        bool has_passed_initial_check = false;
        bool has_loaded_dynamic_maps = false;

        void check_exists()
        {
            const auto path = std::filesystem::current_path() / "config\\config.json";

            std::cout << "[+] Verifying config exists at " << path << "... ";
            if (exists(path)) { std::cout << "Done.\n"; }
            else {
                std::cout << "\n\t[-] Configuration files not found, creating... ";
                create_directories(path.parent_path());
                std::ofstream f(path);
                if (!f.is_open()) {
                    std::cerr << "\n\t[!] Failed to create the config files\n";
                }
                else {
                    f << std::setw(4) << json::parse(required_data);
                    std::cout << "Done.\n";
                    f.close();
                }
            }
        }
    }

    json& get_data()
    {
        const auto path = std::filesystem::current_path() / "config\\config.json";
        if (!has_passed_initial_check) {
            check_exists();
            has_passed_initial_check = true;
        }

        std::ifstream f(path);
        static json d = json::parse(f);
        f.close();

        if (!has_loaded_dynamic_maps) {
            has_loaded_dynamic_maps = true;
            for (auto& manager : bots::drops::managers.get()) {
                if (!bots::drops::configs.contains(manager)) {
                    bots::drops::configs[manager] = config::ManagedVar<
                        llpp::bots::drops::CrateManagerConfig>(
                        {"bots", "drops", manager}, save,
                        llpp::bots::drops::CrateManagerConfig());
                }
            }
        }

        return d;
    }

    void save()
    {
        const auto path = std::filesystem::current_path() / "config\\config.json";

        std::ofstream f(path);
        f << std::setw(4) << get_data();
        f.close();
    }
}

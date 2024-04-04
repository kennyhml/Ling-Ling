#include "config.h"
#include <fstream>
#include "required.h"
#include <ShlObj.h>
#include <iostream>

namespace llpp::config
{
    namespace
    {
        bool has_passed_initial_check = false;
        bool has_loaded_dynamic_maps = false;

        std::filesystem::path get_appdata_path()
        {
            PWSTR path;
            auto res = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path);
            return path;
        }

        void check_exists()
        {
            const auto path = get_appdata_path() / "llpp\\config\\config.json";

            std::cout << "[+] Verifying config exists at " << path << "... ";
            if (exists(path)) { std::cout << "Done.\n"; } else {
                std::cout << "\n\t[-] Configuration files not found, creating... ";
                create_directories(path.parent_path());
                std::ofstream f(path);
                if (!f.is_open()) {
                    std::cerr << "\n\t[!] Failed to create the config files\n";
                } else {
                    f << std::setw(4) << nlohmann::ordered_json::parse(required_data);
                    std::cout << "Done.\n";
                    f.close();
                }
            }
        }
    }

    nlohmann::ordered_json& get_data()
    {
        const auto path = get_appdata_path() / "llpp\\config\\config.json";
        if (!has_passed_initial_check) {
            check_exists();
            has_passed_initial_check = true;
        }

        std::ifstream f(path);
        // NOTE: The object is static, it is only loaded ONCE.
        static nlohmann::ordered_json d = nlohmann::ordered_json::parse(f);
        f.close();

        if (!has_loaded_dynamic_maps) {
            has_loaded_dynamic_maps = true;
            for (auto& manager: bots::drops::managers.get()) {
                if (!bots::drops::configs.contains(manager)) {
                    bots::drops::configs[manager] = ManagedVar(
                        {"bots", "drops", manager}, save,
                        llpp::bots::drops::CrateManagerConfig());
                }
            }
            for (auto& manager: bots::farm::managers.get()) {
                if (!bots::farm::configs.contains(manager)) {
                    auto& obj = bots::farm::configs[manager] = ManagedVar(
                                    {"bots", "metal", manager}, save,
                                    llpp::bots::farm::FarmConfig());

                    obj.get_ptr()->on_changed = [&obj]() { return obj.save(); };
                }
            }

            for (auto& station: bots::parasaur::stations.get()) {
                if (!bots::parasaur::configs.contains(station)) {
                    auto& obj = bots::parasaur::configs[station] = ManagedVar(
                                    {"bots", "parasaur", station}, save,
                                    llpp::bots::parasaur::ParasaurConfig());
                    obj.get_ptr()->on_changed = [&obj]() { return obj.save(); };
                }
            }
        }

        return d;
    }

    void save()
    {
        const auto path = get_appdata_path() / "llpp\\config\\config.json";

        std::ofstream f(path);
        f << std::setw(4) << get_data();
        f.close();
    }
}

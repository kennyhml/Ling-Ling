#include "data.h"
#include <ShlObj.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "../../common/util.h"

namespace llpp::bots::forges
{
    namespace
    {
        bool has_passed_initial_check = false;

        std::filesystem::path get_appdata_path()
        {
            PWSTR path;
            auto res = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path);
            return path;
        }

        void check_exists()
        {
            const auto path = get_appdata_path() / "llpp\\data\\forges.json";
            if (exists(path)) { return; }

            std::cerr << "\n\t[-] Data files not found, creating... ";
            create_directories(path.parent_path());
            std::ofstream f(path);
            if (!f.is_open()) {
                std::cerr << "\n\t[!] Failed to create the data files\n";
            } else {
                f << std::setw(4) << nlohmann::ordered_json::parse("{}");
                std::cout << "Done.\n";
                f.close();
            }
        }

        nlohmann::ordered_json& get_data()
        {
            const auto path = get_appdata_path() / "llpp\\data\\forges.json";
            if (!has_passed_initial_check) {
                check_exists();
                has_passed_initial_check = true;
            }

            std::ifstream f(path);
            static nlohmann::ordered_json d = nlohmann::ordered_json::parse(f);
            f.close();
            return d;
        }

        void save()
        {
            const auto path = get_appdata_path() / "llpp\\data\\forges.json";
            std::ofstream f(path);
            f << std::setw(4) << get_data();
            f.close();
        }
    }

    void get_cooking(const std::string& of_station, std::string& material_out,
                     int64_t& start_out)
    {
        if (!get_data().contains(of_station)) {
            get_data()[of_station]= {{"material", ""}, {"start", 0}};;
            save();
        }
        material_out = get_data()[of_station]["material"];
        start_out = get_data()[of_station]["start"];
    }

    void set_cooking(const std::string& of_station, const std::string& material)
    {
        get_data()[of_station]["material"] = material;
        if (!material.empty()) {
            get_data()[of_station]["start"] = util::time_t_now();
        }
        save();
    }
}

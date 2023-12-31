#include "config.h"
#include <fstream>

namespace llpp::config
{
    json& get_data()
    {
        std::ifstream f("C:/dev/LLPP/LLPP/src/config/config.json");
        if (!f.is_open()) { throw std::runtime_error("Config not found!"); }

        static json d = json::parse(f);
        return d;
    }

    void save()
    {
        std::ofstream f("C:/dev/LLPP/LLPP/src/config/config.json");
        if (!f.is_open()) { throw std::runtime_error("Config not found!"); }

        f << std::setw(4) << get_data();
        f.close();
    }
}

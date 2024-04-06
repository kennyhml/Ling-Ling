#pragma once
#include <string>

namespace llpp::bots::forges
{
    void get_cooking(const std::string& of_station, std::string& material_out,
                     int64_t& start_out);

    void set_cooking(const std::string& of_station, const std::string& material);
}

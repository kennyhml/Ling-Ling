#pragma once
#include <string>
#include <format>
#include <dpp/dpp.h>
#include "parasaur_station_result.h"

namespace llpp::bots::parasaur
{
    const std::string PARASAUR_THUMBNAIL =
        "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/"
        "6/65/Parasaur.png";

    void send_enemy_detected(const std::string& where, int tag_level);

    dpp::message get_summary_message(std::chrono::seconds total_time_taken,
                                     const std::vector<ParasaurStationResult>& stats);
}

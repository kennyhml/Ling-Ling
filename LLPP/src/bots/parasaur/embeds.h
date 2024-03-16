#pragma once
#include <string>
#include <asapp/structures/cavelootcrate.h>
#include <format>
#include "parasaurmanager.h"

namespace llpp::bots::parasaur
{
    const std::string PARASAUR_THUMBNAIL =
        "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/"
        "6/65/Parasaur.png";

    void send_enemy_detected(const std::string& where,  int tag_level);

    void send_summary_embed(std::chrono::seconds time_taken,
        const std::vector<ParasaurManager::ParasaurGroupStatistics>& stats);
}

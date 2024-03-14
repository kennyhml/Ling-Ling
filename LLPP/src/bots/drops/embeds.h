#pragma once
#include "lootresult.h"
#include "../../core/stationresult.h"
#include "cratemanager.h"
#include <asapp/structures/cavelootcrate.h>
#include <format>

namespace llpp::bots::drops
{
    dpp::message get_looted_message(const core::StationResult& data,
                                    const cv::Mat& loot_image,
                                    asa::structures::CaveLootCrate::Quality drop_quality,
                                    int total_times_looted);

    dpp::message get_reroll_message(const core::StationResult& data,
                                    const cv::Mat& loot_image,
                                    asa::structures::CaveLootCrate::Quality drop_quality,
                                    std::chrono::system_clock::time_point expires,
                                    const std::vector<LootResult>& looted);

    dpp::message get_summary_message(const std::string& manager_name,
                                     std::chrono::seconds time_taken,
                                     const std::vector<CrateManager::CrateGroupStatistics>
                                     & stats, const std::map<std::string, float>&
                                     vault_fill_levels,
                                     std::chrono::system_clock::time_point
                                     next_completion);
}

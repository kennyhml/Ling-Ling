#pragma once
#include <dpp/dpp.h>
#include <opencv2/core/mat.hpp>


namespace llpp::bots::boss
{
    dpp::message get_brood_defeated_message(std::chrono::seconds fight_time,
                                            std::chrono::seconds total_time,
                                            const cv::Mat& element_loot_image,
                                            bool element_looted,
                                            bool element_in_dedi);
}

#pragma once
#include <opencv2/core/mat.hpp>

#include "../../core/stationresult.h"

namespace llpp::bots::crafting
{
    void send_spark_crafted(const core::StationResult& data);
    void send_gunpowder_crafted(const core::StationResult& data);
    void send_arb_crafted(const core::StationResult& data);
    void send_paste_grinded(const core::StationResult& data, bool grinder_emptied);

    void send_station_capped(const std::string& station_name, const cv::Mat& ss);
}

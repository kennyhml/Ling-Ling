#pragma once

#include <opencv2/core/mat.hpp>
#include "../../core/stationresult.h"

namespace llpp::bots::crafting
{
    void send_spark_crafted(const core::StationResult& data, bool was_crafting);

    void send_gunpowder_crafted(const core::StationResult& data, bool was_crafting);

    void send_arb_crafted(const core::StationResult& data, bool was_crafting);

    void send_paste_grinded(const core::StationResult& data, bool grinder_emptied);

    void send_station_capped(const std::string& station_name, const cv::Mat& ss);

    void send_forge_cycled(const core::StationResult& data, const std::string& item,
                           int slots_added);

    void send_forge_dedis_capped(const core::StationResult& data);

    void send_forge_dedis_empty(const core::StationResult& data);
}

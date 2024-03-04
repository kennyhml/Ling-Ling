#pragma once
#include <asapp/items/item.h>
#include "../../core/stationresult.h"

namespace llpp::bots::kitchen
{
    void send_crops_collected(const core::StationResult&, const asa::items::Item* item,
                              int slots_in_fridge);

    void send_sap_collected(const core::StationResult&, int slots_in_storage);

    void send_saptap_not_accessible(const std::string& station_name);

    void send_sapstorage_not_accessible(const std::string& station_name);
}

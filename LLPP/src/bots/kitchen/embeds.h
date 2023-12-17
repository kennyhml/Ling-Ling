#pragma once
#include <asapp/items/item.h>
#include "../../core/stationresult.h"

namespace llpp::bots::kitchen
{
    void send_success_embed(const core::StationResult&, const asa::items::Item* item,
                            int slots_in_fridge);
}

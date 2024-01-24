#pragma once
#include "../../core/stationresult.h"

namespace llpp::bots::paste
{
    void send_paste_collected(const core::StationResult& data);
    void send_paste_not_deposited(const std::string& station);

    void send_achatina_not_accessible(const std::string& station_name,
                                      const std::string& achatina_name);
}

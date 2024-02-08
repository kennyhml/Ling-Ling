#include "parasaurstation.h"
#include <asapp/entities/localplayer.h>

namespace llpp::bots::alert
{

    ParasaurStation::ParasaurStation(std::string t_name, std::chrono::minutes t_interval,
                                     bool t_is_bed)
            : BaseStation(std::move(t_name), t_interval), is_bed_station_(t_is_bed),
              bed_(name_), teleporter_(name_), parasaur_(name_ + "::PARASAUR") {}


    core::StationResult ParasaurStation::complete()
    {
        const auto start = std::chrono::system_clock::now();

        if (is_bed_station_) { asa::entities::local_player->fast_travel_to(bed_); }
        else { asa::entities::local_player->teleport_to(teleporter_); }


        return {this, false, util::get_elapsed<std::chrono::seconds>(start), {}};

    }


}
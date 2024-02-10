#include "sapstation.h"
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/tribemanager.h>
#include "../../core/discord.h"
#include "embeds.h"

namespace llpp::bots::kitchen
{
    SapStation::SapStation(std::string t_name, const std::chrono::minutes t_interval) :
        BedStation(std::move(t_name), t_interval), storage_box_(name_ + "::STORAGE", 45),
        tap_(name_ + "::TAP", 1) {}

    core::StationResult SapStation::complete()
    {
        if (!begin()) {
            return {this, false, get_time_taken<std::chrono::seconds>(), {}};
        }

        if (!take_sap()) {
            set_completed();
            return {this, false, get_time_taken<std::chrono::seconds>(), {}};
        }

        put_away_sap();
        set_completed();
        core::StationResult res(this, true, get_time_taken<std::chrono::seconds>(), {});
        send_sap_collected(res, storage_box_slots_);
        return res;
    }

    bool SapStation::take_sap() const
    {
        asa::entities::local_player->access(tap_);
        if (tap_.get_inventory()->slots[0].is_empty()) {
            tap_.get_inventory()->close();
            // send a warning, interval too low? no sap in the tap at least
            return false;
        }

        tap_.get_inventory()->transfer_all();
        tap_.get_inventory()->close();
        return true;
    }

    void SapStation::put_away_sap()
    {
        asa::core::sleep_for(std::chrono::seconds(1));
        asa::entities::local_player->turn_right();
        asa::entities::local_player->access(storage_box_);
        asa::entities::local_player->get_inventory()->transfer_all();
        asa::core::sleep_for(std::chrono::seconds(2));
        storage_box_slots_ = storage_box_.get_current_slots();
        asa::entities::local_player->get_inventory()->close();
        asa::core::sleep_for(std::chrono::seconds(1));
        asa::entities::local_player->crouch();
    }
}

#include "sapstation.h"
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include <asapp/structures/exceptions.h>
#include "../../discord/embeds.h"
#include "embeds.h"
#include "../../discord/bot.h"

namespace llpp::bots::kitchen
{
    SapStation::SapStation(std::string t_name,
                           const std::chrono::system_clock::time_point t_last_completed,
                           const std::chrono::minutes t_interval) :
        BedStation(std::move(t_name), t_last_completed, t_interval),
        storage_box_(name_ + "::STORAGE", 45), tap_(name_ + "::TAP", 1) {}

    core::StationResult SapStation::complete()
    {
        if (!begin()) {
            return {this, false, get_time_taken<std::chrono::seconds>(), {}};
        }

        bool success = false;
        try {
            if (take_sap()) {
                put_away_sap();
                success = true;
            }
        }
        catch (asa::structures::StructureNotOpenedError& e) {
            discord::get_bot()->message_create(discord::create_error_message(e));
        }
        set_completed();
        return {this, success, get_time_taken<std::chrono::seconds>(), {}};
    }

    bool SapStation::take_sap() const
    {
        asa::entities::local_player->access(tap_);
        if (tap_.get_inventory()->slots[0].is_empty()) {
            tap_.get_inventory()->close();
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

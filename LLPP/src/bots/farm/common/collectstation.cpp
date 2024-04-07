#include "collectstation.h"

#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/hud.h>

namespace llpp::bots::farm
{
    namespace
    {
        void dedi_deposit()
        {
            asa::core::sleep_for(300ms);
            asa::controls::press(asa::settings::use);
            asa::core::sleep_for(300ms);
        }
    }

    CollectStation::CollectStation(const std::string& t_name) : BedStation(
        t_name, std::chrono::minutes(5)), vault_(t_name + "::VAULT", 350) {}

    core::StationResult CollectStation::complete()
    {
        asa::entities::local_player->suicide();

        if (!begin(false)) {
            set_completed();
            return {this, false, get_time_taken<>(), {}};
        }
        // This is most likely inside our main crafting area, so lets give it some
        // time to load properly
        asa::core::sleep_for(3s);
        get_whip();

        // Look down to whip the metal up, didnt really seem to matter too much
        // as far as I can tell but just feels most natural.
        asa::entities::local_player->set_pitch(90);
        asa::entities::local_player->primary_attack();
        asa::core::sleep_for(1s);
        asa::entities::local_player->set_pitch(0);

        put_away_whip();
        deposit();

        // crouch to give us access to the bed below the teleporter.
        asa::entities::local_player->crouch();
        return {this, true, get_time_taken<>(), {}};
    }

    void CollectStation::get_whip() const
    {
        asa::entities::local_player->turn_right(180, 500ms);
        asa::entities::local_player->access(vault_);

        // Should catch this from the outer scope and send a message to discord asking
        // for someone to pick up the metal and refill the vault.
        if (vault_.get_inventory()->slots[0].is_empty()) {
            throw std::exception("No whips in the vault");
        }
        vault_.get_inventory()->take_slot(0);

        asa::entities::local_player->get_inventory()->select_slot(1);
        // Press "E" on the whip until it equips and the inventory automatically closes,
        // if the whip happens to be broken it will be repaired first granted dedis nearby
        // have the resources.
        while (vault_.get_inventory()->is_open()) {
            asa::controls::press(asa::settings::use);
            asa::core::sleep_for(1s);
        }
        asa::core::sleep_for(1s);
    }

    void CollectStation::put_away_whip() const
    {
        asa::entities::local_player->access(vault_);
        asa::entities::local_player->get_inventory()->transfer_all("whip");
        asa::core::sleep_for(1s);
        vault_.get_inventory()->close();
        asa::core::sleep_for(2s);
    }

    void CollectStation::deposit() const
    {
        asa::entities::local_player->crouch();
        asa::entities::local_player->set_pitch(30);
        asa::entities::local_player->set_yaw(0);

        dedi_deposit();
        asa::entities::local_player->stand_up();
        asa::entities::local_player->set_pitch(10);
        dedi_deposit();
        asa::entities::local_player->set_pitch(-20);
        dedi_deposit();
    }
}

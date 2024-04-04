#include "woodmanager.h"
#include "../../../common/util.h"
#include "../../../config/config.h"
#include <asapp/core/state.h>
#include <asapp/interfaces/hud.h>
#include <asapp/items/items.h>

namespace llpp::bots::farm
{
    namespace
    {
        void deposit()
        {
            asa::core::sleep_for(500ms);
            asa::controls::press(asa::settings::use);
            asa::core::sleep_for(500ms);
        }
    }

    WoodManager::WoodManager(FarmConfig* t_config)
        : config_(t_config), start_bed_(config_->prefix + "::START", 5min),
          start_tp_(config_->prefix + "::START", 0min),
          unload_tp(config_->prefix + "::UNLOAD", 0min),
          fabricator_(config_->prefix + "::FABRICATOR", 70)
    {
        // We dont need any cooldown on the metal stations because the manager
        // handles that part, the stations themselves have no cooldown as they use
        // teleporters, not beds.
        for (int i = 0; i < config_->num_stations; i++) {
            const std::string name = util::add_num_to_prefix(config_->prefix, i + 1);
            stations_.push_back(std::make_unique<WoodStation>(name));
        }
    }

    bool WoodManager::run()
    {
        if (!is_ready_to_run()) { return false; }

        start_bed_.complete();
        bool at_start = true;
        int current_station = 0;

        while (current_station < stations_.size()) {
            if (at_start) { get_chainsaw(); }

            const auto result = stations_[current_station]->complete();
            const bool was_blackweight = asa::interfaces::hud->is_player_capped();

            //The teleport either failed entirely, in which case we just skip the station
            if (!result.success) {
                current_station++;
                continue;
            }
            at_start = false;

            // We dont want to unload unless blackweighted, unless its the last
            // station anyways, then its fine to unload.
            if (!was_blackweight && current_station != stations_.size() - 1) {
                current_station++;
                continue;
            }

            unload();
            start_tp_.set_default_destination(true);
            start_tp_.complete();
            at_start = true;

            // if this was the last station, exit now if we didnt get blackweighted
            // on the previous tree (meaning its done)
            if (current_station == stations_.size() - 1) {
                current_station += !was_blackweight;
            }
        }
        config_->last_completed = util::time_t_now();
        if (config_->on_changed) { config_->on_changed(); }

        // Put the chainsaw / gas away, we are done.
        asa::entities::local_player->access(fabricator_);
        asa::entities::local_player->get_inventory()->drop_all("thatch");
        asa::core::sleep_for(1s);
        asa::entities::local_player->get_inventory()->transfer_all();
        fabricator_.get_inventory()->close();
        asa::core::sleep_for(1s);
        asa::entities::local_player->crouch();
        return true;
    }

    void WoodManager::get_chainsaw() const
    {
        asa::entities::local_player->access(fabricator_);

        if (asa::entities::local_player->get_inventory()->find_item(
            *asa::items::resources::thatch)) {
            asa::entities::local_player->get_inventory()->drop_all("thatch");
        }

        // put everything on us in the fabricator, so that we can reuse this behavior
        // even if the chainsaw / some gasoline was previously on our local player.
        while (!asa::entities::local_player->get_inventory()->slots[1].is_empty()) {
            asa::entities::local_player->get_inventory()->transfer_all();
            asa::core::sleep_for(500ms);
        }

        auto saw = fabricator_.get_inventory()->find_item(
            *asa::items::weapons::chainsaw);

        if (!saw) { throw std::exception("No chainsaw found in the fabricator."); }

        fabricator_.get_inventory()->select_slot(*saw);
        asa::controls::press(asa::settings::use);
        asa::core::sleep_for(3s);
        fabricator_.get_inventory()->take_slot(*saw);
        asa::core::sleep_for(1s);

        const auto gas = fabricator_.get_inventory()->find_item(
            *asa::items::resources::gasoline);
        if (!gas) { throw std::exception("No gasoline found in the fabricator."); }
        fabricator_.get_inventory()->take_slot(*gas);

        saw = asa::entities::local_player->get_inventory()->find_item(
            *asa::items::weapons::chainsaw, false, false);

        asa::entities::local_player->get_inventory()->select_slot(*saw);
        while (fabricator_.get_inventory()->is_open()) {
            asa::controls::press(asa::settings::use);
            asa::core::sleep_for(1s);
        }
    }

    void WoodManager::unload()
    {
        unload_tp.set_default_destination(true);
        unload_tp.complete();
        asa::core::sleep_for(5s);

        asa::entities::local_player->crouch();

        asa::entities::local_player->turn_down(40);
        asa::entities::local_player->turn_left(20);
        deposit();
        asa::entities::local_player->turn_right(40);
        deposit();

        asa::entities::local_player->stand_up();
        deposit();
        asa::entities::local_player->turn_left(40);
        deposit();

        asa::entities::local_player->turn_up(60);
        deposit();
        asa::entities::local_player->turn_right(40);
        deposit();

        asa::entities::local_player->set_yaw(0);
        asa::entities::local_player->set_pitch(0);
    }

    bool WoodManager::is_ready_to_run()
    {
        if (config_->disabled || config::bots::farm::disabled.get()
            || !start_bed_.is_ready()) {
            return false;
        }

        return util::timedout(
            std::chrono::system_clock::from_time_t(config_->last_completed),
            std::chrono::minutes(config_->interval));
    }

    std::chrono::minutes WoodManager::get_time_left_until_ready() const
    {
        const auto next = std::chrono::system_clock::from_time_t(config_->last_completed)
                          + std::chrono::minutes(config_->interval);

        return util::get_time_left_until<std::chrono::minutes>(next);
    }
}

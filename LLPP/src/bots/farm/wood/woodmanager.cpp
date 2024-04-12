#include "woodmanager.h"

#include <iostream>

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
            asa::core::sleep_for(300ms);
            asa::controls::press(asa::settings::use);
            asa::core::sleep_for(300ms);
        }

        bool is_chainsaw_low_durability()
        {
            asa::entities::local_player->get_inventory()->open();
            const auto slot = asa::entities::local_player->get_inventory()->find_item(
                *asa::items::weapons::chainsaw);

            float durability = 0.f;
            if (slot->get_item_durability(durability)) {
                std::cout << "Chainsaw dura: " << durability * 100 << "%" << std::endl;
            } else { std::cerr << "Failed to check chainsaw durability." << std::endl; }
            asa::entities::local_player->get_inventory()->close();
            asa::core::sleep_for(1s);
            return durability <= 0.4;
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
        start_tp_.set_default_destination(true);

        bool at_start = true;
        int current_station = 0;

        while (current_station < stations_.size()) {
            if (at_start) { get_chainsaw(); }

            // Station was not teleported to, just skip it entirely and go next.
            if (!stations_[current_station]->complete().success) {
                current_station++;
                continue;
            }
            at_start = false;
            // Whether we are blackweighted or not lets us determine if we need
            // to go back to the station we were just at, if we finished without being
            // capped on wood then that means the tree was fully harvested
            const bool was_blackweight = asa::interfaces::hud->is_player_capped();

            // We dont want to unload unless blackweighted, UNLESS its the last
            // station anyways, then its fine to unload.
            if (!was_blackweight && current_station != stations_.size() - 1) {
                current_station++;
                continue;
            }
            unload();
            // To avoid repairing the chainsaw every single time at like ~70% of its dura
            // check if its below 40% and repair only then.
            if (is_chainsaw_low_durability()) {
                start_tp_.complete();
                at_start = true;
            }
            // Make sure to handle the case where we did not cap at the last station but
            // also have no more stations to run.
            if (current_station == stations_.size() - 1) {
                current_station += !was_blackweight;
            }
        }
        config_->last_completed = util::time_t_now();
        if (config_->on_changed) { config_->on_changed(); }
        if (!at_start) { start_tp_.complete(); }
        deposit_chainsaw();
        return true;
    }

    void WoodManager::get_chainsaw() const
    {
        asa::entities::local_player->access(fabricator_);
        fabricator_.get_inventory()->switch_to(
            asa::interfaces::CraftingInventory::INVENTORY);

        if (asa::entities::local_player->get_inventory()->find_item(
            *asa::items::resources::thatch)) {
            asa::entities::local_player->get_inventory()->drop_all("thatch");
            asa::core::sleep_for(500ms);
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

    void WoodManager::deposit_chainsaw() const
    {
        asa::entities::local_player->access(fabricator_);
        asa::entities::local_player->get_inventory()->drop_all("thatch");
        asa::core::sleep_for(1s);
        asa::entities::local_player->get_inventory()->transfer_all();
        fabricator_.get_inventory()->close();
        asa::core::sleep_for(1s);

        // crouch so that we can access the bed below the tp
        asa::entities::local_player->crouch();
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

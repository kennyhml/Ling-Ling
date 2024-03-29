#include <asapp/core/state.h>
#include <asapp/interfaces/hud.h>
#include <asapp/items/items.h>

#include "metalmanager.h"
#include "../../common/util.h"

namespace llpp::bots::metal
{
    MetalManager::MetalManager(MetalManagerConfig* t_config)
        : config_(t_config),
          mount_bed_(config_->prefix + "::MOUNT", std::chrono::minutes(5)),
          collect_bed_(config_->prefix + "::COLLECT", std::chrono::minutes(5)),
          unload_tp_(config_->prefix + "::UNLOAD", std::chrono::minutes(0)),
          start_tp_(config_->prefix + "::START", std::chrono::minutes(0)),
          anky_(std::make_shared<asa::entities::DinoEntity>(config_->prefix + "::ANKY"))
    {
        // We dont need any cooldown on the metal stations because the manager
        // handles that part, the stations themselves have no cooldown as they use
        // teleporters, not beds.
        constexpr std::chrono::minutes cd(0);
        for (int i = 0; i < config_->num_stations; i++) {
            const std::string name = util::add_num_to_prefix(config_->prefix, i + 1);
            stations_.push_back(std::make_unique<MetalStation>(name, cd, i == 0, anky_));
        }

        dedis_.emplace_back(config_->prefix + "::METAL DEDI01");
        dedis_.emplace_back(config_->prefix + "::METAL DEDI02");
    }

    bool MetalManager::run()
    {
        if (!is_ready_to_run()) { return false; }

        // mounting the anky failed, but we still technically ran the manager.
        if (!mount_anky()) { return true; }

        for (const auto& station : stations_) {
            station->complete();
        }

        unload();
        collect();

        return true;
    }

    bool MetalManager::is_ready_to_run()
    {
        if (!mount_bed_.is_ready()) { return false; }

        return util::timedout(
            std::chrono::system_clock::from_time_t(config_->last_completed),
            std::chrono::minutes(config_->interval_minutes_));
    }

    std::chrono::minutes MetalManager::get_time_left_until_ready() const
    {
        const auto next = std::chrono::system_clock::from_time_t(config_->last_completed)
            + std::chrono::minutes(config_->interval_minutes_);

        return util::get_time_left_until<std::chrono::minutes>(next);
    }

    bool MetalManager::mount_anky()
    {
        if (!mount_bed_.complete().success) { return false; }
        asa::entities::local_player->turn_up(90, 1s);

        asa::entities::local_player->mount(*anky_);
        asa::entities::local_player->set_pitch(90);
        return true;
    }

    void MetalManager::unload()
    {
        unload_tp_.set_default_destination(true);
        unload_tp_.complete();

        anky_->open_inventory();
        asa::core::sleep_for(std::chrono::milliseconds(500));
        anky_->get_inventory()->search_bar.search_for("metal");
        asa::core::sleep_for(std::chrono::milliseconds(500));

        const auto pos = anky_->get_inventory()->slots[0].area.get_random_location(4);
        click_at(pos, asa::controls::LEFT);

        anky_->get_inventory()->toggle_tooltips();
        anky_->get_inventory()->popcorn_all();
        anky_->get_inventory()->toggle_tooltips();
        anky_->get_inventory()->close();

        start_tp_.set_default_destination(true);
        start_tp_.complete();
    }

    void MetalManager::collect()
    {
        asa::entities::local_player->suicide();
        collect_bed_.complete();

        asa::entities::local_player->crouch();
        asa::entities::local_player->set_pitch(90);
        asa::core::sleep_for(500ms);

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 12; j++) {
                asa::entities::local_player->pick_up_all();
                asa::entities::local_player->turn_right(360 / 12);
                asa::entities::local_player->pick_up_all();
            }
            asa::entities::local_player->turn_up(45 / 4);
        }

        asa::entities::local_player->set_pitch(30);
        asa::entities::local_player->set_yaw(0);

        asa::controls::press(asa::settings::use);
        asa::entities::local_player->set_pitch(30);
        asa::controls::press(asa::settings::use);
    }
}

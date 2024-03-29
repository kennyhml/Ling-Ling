#include <asapp/core/state.h>
#include <asapp/items/items.h>

#include "metalmanager.h"
#include "../../common/util.h"

namespace llpp::bots::metal
{
    MetalManager::MetalManager(MetalManagerConfig* t_config)
        : config_(t_config),
          anky_(std::make_shared<asa::entities::DinoEntity>(config_->prefix + "::ANKY")),
          mount_bed_(config_->prefix + "::MOUNT", 5min),
          start_tp_(config_->prefix + "::START", 0min),
          unload_station_(config_->prefix + "::UNLOAD", anky_),
          collect_station_(config_->prefix + "::COLLECT")
    {
        // We dont need any cooldown on the metal stations because the manager
        // handles that part, the stations themselves have no cooldown as they use
        // teleporters, not beds.
        constexpr std::chrono::minutes cd(0);
        for (int i = 0; i < config_->num_stations; i++) {
            const std::string name = util::add_num_to_prefix(config_->prefix, i + 1);
            stations_.push_back(std::make_unique<MetalStation>(name, cd, i == 0, anky_));
        }
    }

    bool MetalManager::run()
    {
        if (!is_ready_to_run()) { return false; }

        if (!mount_anky()) {
            // Make sure we suicide, we wont be able to use the bed below the anky.
            asa::entities::local_player->suicide();
            return true;
        }
        for (const auto& station : stations_) {
            station->complete();
        }

        // TODO: Add error handling here, e.g if unload fails act accordingly.
        unload_station_.complete();
        start_tp_.complete();
        collect_station_.complete();
        return true;
    }

    bool MetalManager::is_ready_to_run()
    {
        if (!(mount_bed_.is_ready() && collect_station_.is_ready())) { return false; }

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
}

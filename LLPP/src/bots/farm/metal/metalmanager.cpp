#include "metalmanager.h"

#include <iostream>

#include "../../../common/util.h"
#include "../../../config/config.h"
#include <asapp/interfaces/console.h>
#include <asapp/core/state.h>

#include "commands.h"
#include "../../../discord/bot.h"
#include "../../../discord/embeds.h"
#include "../common/exceptions.h"

namespace llpp::bots::farm
{
    MetalManager::MetalManager(FarmConfig* t_config)
        : config_(t_config),
          anky_(std::make_shared<asa::entities::DinoEntity>(config_->prefix + "::ANKY")),
          start_tp_(config_->prefix + "::START", 0min),
          load_tp_(config_->prefix + "::LOAD", 0min),
          unload_station_(config_->prefix + "::UNLOAD", "metal", anky_),
          collect_station_(config_->prefix + "::COLLECT"),
          mount_station_(config_->prefix + "::MOUNT", anky_, nullptr,
                         std::make_unique<core::BedStation>(
                             config_->prefix + "::REPOSITION", 0min))
    {
        // We dont need any cooldown on the metal stations because the manager
        // handles that part, the stations themselves have no cooldown as they use
        // teleporters, not beds.
        constexpr std::chrono::minutes cd(0);
        for (int i = 0; i < config_->num_stations; i++) {
            const std::string name = util::add_num_to_prefix(config_->prefix, i + 1);
            stations_.push_back(std::make_unique<MetalStation>(name, cd, anky_));
        }
    }

    bool MetalManager::run()
    {
        if (!is_ready_to_run()) { return false; }
        if (!mount_station_.complete().success) {
            // Make sure we suicide, we wont be able to use the bed below the anky.
            asa::entities::local_player->suicide();
            return false;
        }

        load_tp_.set_default_destination(true);
        load_tp_.complete();
        asa::core::sleep_for(15s);

        for (const auto& station: stations_) {
            try {
                station->complete();
            } catch (const NotOnTeleporterError& e) {
                std::cerr << e.what() << std::endl;
                config_->disabled = true;
                config_->on_changed();
                const auto msg = discord::create_error_message(
                    std::format("Fell off the Teleporter at `{}`.", station->get_name()));
                const_cast<dpp::embed&>(msg.embeds[0]).set_description(std::format(
                    "`{}` has been automatically disabled."
                    "\nBring the anky to `{}` and re-enable it.", config_->prefix,
                    start_tp_.get_name()));
                discord::get_bot()->message_create(msg);
                asa::entities::local_player->suicide();
                return true;
            }
        }

        // Save here already since the metal is now farmed.
        config_->last_completed = util::time_t_now();
        if (config_->on_changed) { config_->on_changed(); }

        // TODO: Add error handling here, e.g if unload fails act accordingly.
        unload_station_.complete();
        start_tp_.set_default_destination(true);
        start_tp_.complete();
        collect_station_.complete();
        return true;
    }

    bool MetalManager::is_ready_to_run()
    {
        if (config_->disabled || config::bots::farm::disabled.get()
            || !(mount_station_.is_ready() && collect_station_.is_ready())) {
            return false;
        }

        return util::timedout(
            std::chrono::system_clock::from_time_t(config_->last_completed),
            std::chrono::minutes(config_->interval));
    }

    std::chrono::minutes MetalManager::get_time_left_until_ready() const
    {
        const auto next = std::chrono::system_clock::from_time_t(config_->last_completed)
                          + std::chrono::minutes(config_->interval);

        return util::get_time_left_until<std::chrono::minutes>(next);
    }

    std::vector<std::unique_ptr<MetalManager> > create_metal_managers()
    {
        register_metal_commands();

        std::vector<std::unique_ptr<MetalManager> > ret;
        for (auto& [name, config]: config::bots::farm::configs) {
            if (config.get_ptr()->type == "METAL") {
                ret.emplace_back(std::make_unique<MetalManager>(config.get_ptr()));
            }
        }
        return ret;
    }
}

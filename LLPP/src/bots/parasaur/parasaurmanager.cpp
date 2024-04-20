#include "parasaurmanager.h"
#include "parasaur_station_result.h"
#include "embeds.h"
#include "../../config/config.h"
#include "../../common/util.h"
#include <asapp/core/state.h>
#include "../../discord/bot.h"

namespace llpp::bots::parasaur
{
    using namespace config::bots::parasaur;

    ParasaurManager::ParasaurManager()
        : spawn_bed_(teleport_start.get(), 5min), spawn_tp_(teleport_start.get(), 5min)
    {
        for (auto& [name, data]: configs) {
            if (data.get_ptr()->is_teleporter) {
                tp_stations_.emplace_back(
                    std::make_unique<TeleportParasaurStation>(name, data.get_ptr()));
            } else {
                bed_stations_.emplace_back(
                    std::make_unique<BedParasaurStation>(name, data.get_ptr()));
            }
        }
        update_dashboard_component();
    }

    bool ParasaurManager::run()
    {
        if (!is_ready_to_run()) { return false; }

        std::vector<ParasaurStationResult> results;
        bool any_ran = false;

        bool started_teleporters = false;
        const auto start = std::chrono::system_clock::now();

        for (const auto& station: tp_stations_) {
            if (!station->is_ready()) {
                results.emplace_back(false, station.get(), station->get_last_alert());
                continue;
            }
            if (!started_teleporters) {
                if (!go_to_start()) { return false; }
                started_teleporters = true;
            }
            any_ran = true;
            station->complete();
            results.emplace_back(true, station.get(), station->get_last_alert());
        }

        if (started_teleporters) { spawn_tp_.complete(); }

        for (const auto& station: bed_stations_) {
            if (station->is_ready()) {
                any_ran = true;
                station->complete();
                results.emplace_back(true, station.get(), station->get_last_alert());
            } else {
                results.emplace_back(false, station.get(), station->get_last_alert());
            }
        }
        if (any_ran) {
            const dpp::message summary = get_summary_message(
                util::get_elapsed<std::chrono::seconds>(start), results);
            discord::get_bot()->message_create(summary);
        }
        last_completed_ = std::chrono::system_clock::now();
        return any_ran;
    }

    bool ParasaurManager::is_ready_to_run()
    {
        if (disabled.get() || !(spawn_bed_.is_ready())) {
            return false;
        }

        if (start_criteria.get() == "INTERVAL") {
            return util::timedout(last_completed_,
                                  std::chrono::minutes(start_interval.get()));
        }

        if (start_criteria.get() == "MIN READY") {
            int ready = 0;
            for (const auto& station: tp_stations_) { ready += station->is_ready(); }
            for (const auto& station: bed_stations_) { ready += station->is_ready(); }
            return ready >= start_min_ready.get();
        }

        std::cerr << "[!] Parasaur start criteria was not matched!" << std::endl;
        return false;
    }

    std::chrono::minutes ParasaurManager::get_time_left_until_ready() const
    {
        return util::get_time_left_until<std::chrono::minutes>(
            tp_stations_[0]->get_next_completion());
    }

    bool ParasaurManager::go_to_start()
    {
        if (!spawn_bed_.complete().success) { return false; }
        asa::core::sleep_for(std::chrono::seconds(start_load.get()));
        return true;
    }

    void ParasaurManager::update_dashboard_component()
    {
        if (!dashboard_field_) {
            dashboard_field_ = std::make_shared<dpp::embed_field>();
            discord::register_dashboard_field(dashboard_field_);
        }
        std::string next = "**`Now`**";
        if (const auto time_left = get_time_left_until_ready(); time_left.count() > 0) {
            next = std::format("**<t:{}:R>**", next);
        }
        std::string last = "**`-`**";
        if (last_completed_ != std::chrono::system_clock::time_point()) {
            last = std::format("**<t:{}:R>**", last_completed_);
        }

        dashboard_field_->name = "__Parasaurs:__";
        dashboard_field_->value = std::format(
            ">>> Last: {}"
            "\nNext: {}", last, next);
        dashboard_field_->is_inline = true;
    }
}

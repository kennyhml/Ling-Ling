#include "parasaurmanager.h"

#include <asapp/core/init.h>
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>

#include "../../config/config.h"
#include "../../common/util.h"
#include <asapp/interfaces/tribemanager.h>

#include <utility>

#include "embeds.h"
#include "../../discord/bot.h"

namespace llpp::bots::parasaur
{
    using namespace config::bots::parasaur;

    ParasaurManager::ParasaurManager()
        : spawn_bed_(teleport_start.get(), std::chrono::minutes(5)),
          spawn_tp_(teleport_start.get(), std::chrono::minutes(5)),
          next_tp_("PARASAUR::NEXT", std::chrono::minutes(0))
    {
        for (auto& [name, data] : configs) {
            if (data.get_ptr()->is_teleporter) {
                tp_stations_.emplace_back(
                    std::make_unique<TeleportParasaurStation>(name, data.get_ptr()));
            }
            else {
                bed_stations_.emplace_back(
                    std::make_unique<BedParasaurStation>(name, data.get_ptr()));
            }
        }
    }

    void ParasaurManager::ParasaurGroupStatistics::station_checked(const std::string& stationname, bool alerting)
    {
        const auto now = std::chrono::system_clock::now();
        const std::chrono::seconds elapsed = (last_checked != UNDEFINED_TIME)
                                                 ? util::get_elapsed<
                                                     std::chrono::seconds>(last_checked)
                                                 : std::chrono::minutes(30);

        times_checked_++;
        last_checked = now;
        parasaur_station_name = stationname;
        parasaur_alerting = alerting;
    }

    bool ParasaurManager::run()
    {
        bool parasaursChecked = false;

        if (!is_ready_to_run()) { return false; }
        bool started_teleporters = false;

        const auto start = std::chrono::system_clock::now();

        int i = 0;
        std::string station_name;
        bool enemy_detected;
        station_stats.clear();
        station_stats.resize(tp_stations_.size() + bed_stations_.size());
        for (const auto& station : tp_stations_) {
            if (!station->is_ready()) { continue; }

            if (!started_teleporters) {
                if (!go_to_start()) { return false; }
                started_teleporters = true;
            }
            station->complete(station_name, enemy_detected);
            station_stats[i].station_checked(station_name, enemy_detected);
            parasaursChecked = true;
            i++;
        }

        if (started_teleporters) { spawn_tp_.complete(); }

        int j = 0;
        for (const auto& station : bed_stations_) {
            if (station->BedStation::is_ready()) {
                station->complete(station_name, enemy_detected);

                station_stats[i+j].station_checked(station_name, enemy_detected);
                parasaursChecked = true;
                j++;
            }
        }

        if(parasaursChecked) {
            // Only send the embed if a parasaur station was completed
            send_summary_embed(util::get_elapsed<std::chrono::seconds>(start), station_stats);

            last_completed_ = std::chrono::system_clock::now();
        }

        return true;
    }

    bool ParasaurManager::is_ready_to_run()
    {
        if (disabled.get() || !(spawn_bed_.is_ready() && next_tp_.is_ready())) {
            return false;
        }

        if (start_criteria.get() == "INTERVAL") {
            return util::timedout(last_completed_,
                                  std::chrono::minutes(start_interval.get()));
        }

        if (start_criteria.get() == "MIN READY") {
            int ready = 0;
            for (const auto& station : tp_stations_) { ready += station->is_ready(); }
            for (const auto& station : bed_stations_) {
                ready += station->BedStation::is_ready();
            }
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

        asa::entities::local_player->crouch();
        spawn_tp_.set_tp_is_default(true);
        return spawn_tp_.complete().success;
    }
}

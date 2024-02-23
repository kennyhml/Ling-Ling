#include "parasaurmanager.h"

#include <asapp/entities/localplayer.h>

#include "../../config/config.h"
#include "../../common/util.h"
#include <asapp/interfaces/tribemanager.h>
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
            auto station = std::make_unique<ParasaurStation>(name, data.get());
            if (data.get_ptr()->is_teleporter) {
                tp_stations_.push_back(std::move(station));
            }
            else { bed_stations_.push_back(std::move(station)); }
        }
    }

    bool ParasaurManager::run()
    {
        if (!is_ready_to_run()) { return false; }
        bool started_teleporters = false;

        for (const auto& station : tp_stations_) {
            if (!station->BedStation::is_ready()) { continue; }

            if (!started_teleporters) {
                if (!go_to_start()) { return false; }
                started_teleporters = true;
            }
            station->complete();
        }

        if (started_teleporters) { spawn_tp_.complete(); }

        for (const auto& station : bed_stations_) {
            if (station->BedStation::is_ready()) { station->complete(); }
        }
        last_completed_ = std::chrono::system_clock::now();
        return true;
    }

    bool ParasaurManager::is_ready_to_run()
    {
        if (disabled.get()) { return false; }
        if (!spawn_bed_.is_ready() || !next_tp_.is_ready()) { return false; }

        if (start_criteria.get() == "INTERVAL") {
            return util::timedout(last_completed_,
                                  std::chrono::minutes(start_interval.get()));
        }

        if (start_criteria.get() == "MIN READY") {
            int ready = 0;
            for (const auto& station : tp_stations_) {
                ready += station->BedStation::is_ready();
            }
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
            tp_stations_[0]->BedStation::get_next_completion());
    }

    bool ParasaurManager::go_to_start()
    {
        if (!spawn_bed_.complete().success) { return false; }

        asa::entities::local_player->crouch();
        spawn_tp_.set_tp_is_default(true);
        return spawn_tp_.complete().success;
    }
}

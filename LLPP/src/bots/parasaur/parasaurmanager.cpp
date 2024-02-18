#include "parasaurmanager.h"

#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>

#include "../../config/config.h"
#include "../../common/util.h"
#include <asapp/interfaces/tribemanager.h>
#include "../../discord/bot.h"
#include "../../discord/tribelogs/handler.h"

namespace llpp::bots::parasaur
{
    using namespace config::bots::parasaur;

    ParasaurManager::ParasaurManager()
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

        bool any_tp_done = false;

        for (const auto& station : tp_stations_) {
            if (!station->BedStation::is_ready()) { continue; }

            if (!any_tp_done) {
                go_to_start_tp();
                any_tp_done = true;
            }
            station->complete();
        }

        if (any_tp_done) {
            const asa::structures::Teleporter start(teleport_start.get());
            asa::entities::local_player->teleport_to(start);
        }

        for (const auto& station : bed_stations_) {
            if (station->BedStation::is_ready()) { station->complete(); }
        }
        last_completed_ = std::chrono::system_clock::now();
        return true;
    }

    bool ParasaurManager::is_ready_to_run()
    {
        if (start_criteria.get() == "INTERVAL") {
            return util::timedout(last_completed_,
                                  std::chrono::minutes(start_interval.get()));
        }

        if (start_criteria.get() == "MIN READY") {
            int ready = 0;
            for (const auto& station : tp_stations_) { ready += station->BedStation::is_ready(); }
            for (const auto& station : bed_stations_) { ready += station->BedStation::is_ready(); }
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

    void ParasaurManager::go_to_start_tp() const
    {
        const asa::structures::SimpleBed bed(teleport_start.get());

        asa::entities::local_player->fast_travel_to(bed, AccessFlags_Default,
                                                    TravelFlags_NoTravelAnimation);

        asa::interfaces::tribe_manager->update_tribelogs(discord::handle_tribelog_events);
        asa::core::sleep_for(std::chrono::seconds(1));

        const asa::structures::Teleporter next("PARASAUR::START NEXT");
        asa::entities::local_player->teleport_to(next, true);
    }
}

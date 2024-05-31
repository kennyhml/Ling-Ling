#include <iostream>
#include "phoenixmanager.h"

#include "../../common/util.h"
#include "../../config/config.h"


namespace llpp::bots::phoenix
{
    PhoenixManager::PhoenixManager()
    {
        auto interval = std::chrono::duration_cast<std::chrono::minutes>(std::chrono::seconds(config::bots::phoenix::interval.get()));
        for (int i = 0; i < config::bots::phoenix::num_stations.get(); i++) {
            phoenix_stations_.push_back(std::make_unique<PhoenixStation>(
                    util::add_num_to_prefix("PHOENIX", i + 1), interval));
        }
    }


    bool PhoenixManager::run()
    {
        if (!is_ready_to_run()) { return false; }

        for (const auto& station : phoenix_stations_) { station->complete(); }
        return true;
    }

    bool PhoenixManager::is_ready_to_run()
    {
        if (config::bots::phoenix::disabled.get()) { return false; }
        if (!phoenix_stations_.empty()) { return phoenix_stations_[0]->is_ready(); }
        return false;
    }

    std::chrono::minutes PhoenixManager::get_time_left_until_ready() const
    {
        return util::get_time_left_until<std::chrono::minutes>(
                phoenix_stations_[0]->get_next_completion());
    }
}

#include "sapmanager.h"

#include "../../common/util.h"
#include "../../config/config.h"


namespace llpp::bots::kitchen
{
    SapManager::SapManager()
    {
        using namespace llpp::config::bots::sap;
        std::chrono::minutes station_interval(interval.get());
        for (int i = 0; i < num_stations.get(); i++) {
            stations_.push_back(std::make_unique<SapStation>(
                util::add_num_to_prefix(prefix.get(), i + 1), station_interval));
        }
    }

    bool SapManager::run()
    {
        if (!is_ready_to_run() || config::bots::sap::disabled.get()) { return false; }
        for (const auto& station : stations_) { station->complete(); }
        return true;
    }

    bool SapManager::is_ready_to_run() const { return stations_[0]->is_ready(); }

    std::chrono::minutes SapManager::get_time_left_until_ready() const
    {
        return util::get_time_left_until<std::chrono::minutes>(
            stations_[0]->get_next_completion());
    }
}

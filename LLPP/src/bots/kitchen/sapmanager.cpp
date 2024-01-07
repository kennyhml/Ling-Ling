#include "sapmanager.h"

#include "../../common/util.h"
#include "../../config/config.h"


namespace llpp::bots::kitchen
{
    SapManager::SapManager(std::string t_prefix, int t_num_stations,
                           std::chrono::minutes t_interval)
    {
        for (int i = 0; i < t_num_stations; i++) {
            stations_.push_back(
                std::make_unique<SapStation>(util::add_num_to_prefix(t_prefix, i + 1),
                                             t_interval));
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

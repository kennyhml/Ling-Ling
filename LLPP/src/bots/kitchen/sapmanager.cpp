#include "sapmanager.h"
#include "../../common/util.h"
#include "../../config/config.h"


namespace llpp::bots::kitchen
{
    using namespace llpp::config::bots::sap;

    namespace
    {
        void resize_times()
        {
            // avoid saving the vector if we dont have to
            if (times.get_ptr()->size() == num_stations.get()) { return; }

            // resize to the amount of stations we have with 0 as default.
            times.get_ptr()->resize(num_stations.get(), 0);
            times.save();
        }

        std::unique_ptr<SapStation> build_station(const int index)
        {
            return std::make_unique<SapStation>(
                util::add_num_to_prefix(prefix.get(), index + 1),
                std::chrono::system_clock::from_time_t(times.get_ptr()->at(index)),
                std::chrono::minutes(interval.get()));
        }
    }

    SapManager::SapManager()
    {
        resize_times();
        for (int i = 0; i < num_stations.get(); i++) {
            stations_.push_back(std::move(build_station(i)));
        }
    }

    bool SapManager::run()
    {
        for (int i = 0; i < stations_.size(); i++) {
            if (!stations_[i]->is_ready() || disabled.get()) { continue; }

            if (stations_[i]->complete().success) {
                const int64_t now = std::chrono::system_clock::to_time_t(
                    std::chrono::system_clock::now());
                times.get_ptr()->at(i) = now;
                times.save();
                return true;
            }
            // station failed, so instead of setting the last completion time we
            // suspend it for 5 minutes (bed cooldown)
            stations_[i]->suspend_for(std::chrono::minutes(5));
        }
        return false;
    }

    bool SapManager::is_ready_to_run()
    {
        if (disabled.get() || stations_.empty()) { return false; }

        return std::ranges::any_of(stations_, [](const auto& station) {
            return station->is_ready();
        });
    }

    std::chrono::minutes SapManager::get_time_left_until_ready() const
    {
        return util::get_time_left_until<std::chrono::minutes>(
            stations_[0]->get_next_completion());
    }
}

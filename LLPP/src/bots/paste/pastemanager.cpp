#include "pastemanager.h"
#include "../../common/util.h"
#include "../../config/config.h"

namespace llpp::bots::paste
{
    using namespace config::bots::paste;

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

        std::unique_ptr<PasteStation> build_station(const int index)
        {
            return std::make_unique<PasteStation>(
                util::add_num_to_prefix(prefix.get(), index + 1),
                std::chrono::system_clock::from_time_t(times.get_ptr()->at(index)),
                std::chrono::minutes(interval.get()));
        }
    }

    PasteManager::PasteManager()
    {
        resize_times();
        for (int i = 0; i < num_stations.get(); i++) {
            stations_.push_back(std::move(build_station(i)));
        }
    };

    bool PasteManager::run()
    {
        bool any_ran = false;
        for (int i = 0; i < stations_.size(); i++) {
            if (!stations_[i]->is_ready() || disabled.get()) { continue; }

            if (!(any_ran || is_paste_rendered())) {
                if (!render_station_.is_ready() || !render_station_.complete().success) {
                    return false;
                }
            }

            if (stations_[i]->complete().success) {
                any_ran = true;
                times.get_ptr()->at(i) = util::time_t_now();
                times.save();
            }
            else {
                // station failed in a non-fatal matter, suspend it for bed cooldown.
                stations_[i]->suspend_for(std::chrono::minutes(5));
            }

            // If completion is partial, return true to signal a station was completed.
            if (any_ran && allow_partial.get()) { return true; }
        }
        return any_ran;
    }

    bool PasteManager::is_ready_to_run()
    {
        return std::ranges::any_of(stations_, [](const auto& station) {
            return station->is_ready();
        });
    }

    std::chrono::minutes PasteManager::get_time_left_until_ready() const
    {
        return util::get_time_left_until<std::chrono::minutes>(
            stations_[0]->get_next_completion());
    }

    bool PasteManager::is_paste_rendered() const
    {
        auto max = std::ranges::max_element(stations_,
                                            [](const auto& s1, const auto& s2) {
                                                return s1->get_last_completion() < s2->
                                                    get_last_completion();
                                            });
        auto elem = std::distance(stations_.begin(), max);
        if (max != stations_.end()) {
            return util::get_elapsed<std::chrono::seconds>(
                stations_[elem]->get_last_completion()).count() < 40;
        }
        return false;
    }
}

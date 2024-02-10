#include "pastemanager.h"
#include "../../common/util.h"
#include "../../config/config.h"

namespace llpp::bots::paste
{
    PasteManager::PasteManager()
    {
        using namespace llpp::config::bots::paste;

        std::chrono::minutes p_interval(interval.get());
        for (int i = 0; i < num_stations.get(); i++) {
            std::string name = util::add_num_to_prefix(prefix.get(), i + 1);
            paste_stations_.push_back(std::make_unique<PasteStation>(name, p_interval));
        }
    };

    bool PasteManager::run()
    {
        using namespace config::bots::paste;
        bool any_ran = false;

        for (const auto& station: paste_stations_) {
            if (!station->is_ready() || (!render_station_.is_ready() && !
                    is_paste_rendered()) || disable_completion.get()) { continue; }

            if (!is_paste_rendered()) { render_station_.complete(); }

            station->complete();
            any_ran = true;
            // if partial completion is enabled, return true to signal that we finished
            // and another higher priority station is now allowed to be completed.
            if (allow_partial.get()) { return true; }
        }

        return any_ran;
    }

    bool PasteManager::is_ready_to_run()
    {
        return std::any_of(paste_stations_.begin(), paste_stations_.end(),
                           [](const auto& station) {
                               return station->is_ready();
                           });
    }

    std::chrono::minutes PasteManager::get_time_left_until_ready() const
    {
        return util::get_time_left_until<std::chrono::minutes>(
                paste_stations_[0]->get_next_completion());
    }

    const PasteStation* PasteManager::peek_station(int index) const
    {
        if (index > 0 && index < paste_stations_.size()) {
            return paste_stations_[index].get();
        }
        return nullptr;
    }

    bool PasteManager::is_paste_rendered() const
    {
        auto max = std::ranges::max_element(paste_stations_,
                                            [](const auto& s1, const auto& s2) {
                                                return s1->get_last_completion() < s2->
                                                        get_last_completion();
                                            });
        auto elem = std::distance(paste_stations_.begin(), max);
        if (max != paste_stations_.end()) {
            return util::get_elapsed<std::chrono::seconds>(
                    paste_stations_[elem]->get_last_completion()).count() < 40;
        }
        return false;
    }
}

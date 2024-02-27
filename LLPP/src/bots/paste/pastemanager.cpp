#include "pastemanager.h"
#include "../../common/util.h"
#include "../../config/config.h"

namespace llpp::bots::paste
{
    using namespace config::bots::paste;

    PasteManager::PasteManager()
    {
        const int original_size = static_cast<int>(times.get_ptr()->size());
        times.get_ptr()->resize(num_stations.get());
        if (times.get_ptr()->size() > original_size) {
            std::fill(times.get_ptr()->begin() + original_size, times.get_ptr()->end(),
                      "0");
        }
        times.save();
        std::chrono::minutes p_interval(interval.get());
        for (int i = 0; i < num_stations.get(); i++) {
            std::string name = util::add_num_to_prefix(prefix.get(), i + 1);
            auto completed = util::json_to_time(times.get_ref()[i]);
            paste_stations_.push_back(
                std::make_unique<PasteStation>(name, completed, p_interval));
        }
    };

    bool PasteManager::run()
    {
        bool any_ran = false;
        for (int i = 0; i < paste_stations_.size(); i++) {
            // Completion may get disabled during runtime so check every time.
            if (!paste_stations_[i]->is_ready() || disabled.get()) { continue; }

            if (!any_ran && !is_paste_rendered()) {
                if (render_station_.get_state() != core::BaseStation::State::ENABLED) {
                    return false;
                }
                if (!render_station_.complete().success) { return false; }
            }

            if (paste_stations_[i]->complete().success) {
                any_ran = true;
                (*times.get_ptr())[i] = _strdup(
                    util::time_to_json(std::chrono::system_clock::now()).c_str());
                times.save();
            }
            // if partial completion is enabled, return true to signal that we finished
            // and another higher priority station is now allowed to be completed.
            if (any_ran && allow_partial.get()) { return true; }
        }
        return any_ran;
    }

    bool PasteManager::is_ready_to_run()
    {
        return std::ranges::any_of(paste_stations_, [](const auto& station) {
            return station->is_ready();
        });
    }

    std::chrono::minutes PasteManager::get_time_left_until_ready() const
    {
        return util::get_time_left_until<std::chrono::minutes>(
            paste_stations_[0]->get_next_completion());
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

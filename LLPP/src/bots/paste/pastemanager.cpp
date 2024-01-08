#include "pastemanager.h"
#include "../../common/util.h"
#include "../../core/discord.h"
#include "embeds.h"
#include "../../config/config.h"

namespace llpp::bots::paste
{
    PasteManager::PasteManager()
    {
        using namespace llpp::config::bots::paste;

        std::chrono::minutes station_interval(interval.get());
        for (int i = 0; i < num_stations.get(); i++) {
            std::string name = util::add_num_to_prefix(prefix.get(), i + 1);
            stations.push_back(std::make_unique<PasteStation>(name, station_interval));
        }
    };

    bool PasteManager::run()
    {
        if (!is_ready_to_run() || config::bots::paste::disable_completion.get()) {
            return false;
        }

        auto renderStationResult = render_station.complete();
        if (!renderStationResult.get_success()) {
            throw std::runtime_error("Render station failed.");
        }

        for (auto& station : stations) { station->complete(); }
    }

    bool PasteManager::is_ready_to_run() const { return stations[0]->is_ready(); }

    std::chrono::minutes PasteManager::get_time_left_until_ready() const
    {
        return util::get_time_left_until<std::chrono::minutes>(
            stations[0]->get_next_completion());
    }

    const PasteStation* PasteManager::peek_station(int index) const
    {
        if (index > 0 && index < stations.size()) { return stations[index].get(); }
        return nullptr;
    }
}

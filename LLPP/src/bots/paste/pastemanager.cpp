#include "pastemanager.h"
#include "../../common/util.h"
#include "../../core/discord.h"
#include "embeds.h"

namespace llpp::bots::paste
{
    PasteManager::PasteManager(std::string t_prefix, int t_num_stations,
                               std::chrono::minutes t_interval)
    {
        for (int i = 0; i < t_num_stations; i++) {
            std::string name = util::add_num_to_prefix(t_prefix, i + 1);
            stations.push_back(std::make_unique<PasteStation>(name, t_interval));
        }
    };

    bool PasteManager::run()
    {
        if (!is_ready_to_run()) { return false; }

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

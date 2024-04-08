#include "forgemanager.h"

#include "../../common/util.h"
#include "../../config/config.h"

namespace llpp::bots::forges
{
    using namespace llpp::config::bots::forges;

    ForgeManager::ForgeManager()
    {
        loadups_ = std::make_shared<std::vector<LoadupStation> >();
        unloads_ = std::make_shared<std::vector<UnloadStation> >();
        int unload = 0;
        int loadup = 0;
        for (int i = 0; i < metal_loadups.get(); i++) {
            loadups_->emplace_back(util::add_num_to_prefix("LOADUP", ++loadup), "METAL");
            unloads_->emplace_back(util::add_num_to_prefix("UNLOAD", ++unload), "METAL");
            unloads_->emplace_back(util::add_num_to_prefix("UNLOAD", ++unload), "METAL");
        }

        for (int i = 0; i < wood_loadups.get(); i++) {
            loadups_->emplace_back(util::add_num_to_prefix("LOADUP", ++loadup), "WOOD");
            unloads_->emplace_back(util::add_num_to_prefix("UNLOAD", ++unload), "WOOD");
            unloads_->emplace_back(util::add_num_to_prefix("UNLOAD", ++unload), "WOOD");
        }

        for (int i = 0; i < num_stations.get(); i++) {
            const std::string name = util::add_num_to_prefix("FORGE", i + 1);
            const int32_t view_diff = i % 2 == 0 ? 0 : 180;
            stations_.emplace_back(
                std::make_unique<ForgeStation>(name, view_diff, loadups_, unloads_));
        }
    }

    bool ForgeManager::run()
    {
        if (!is_ready_to_run()) { return false; }

        for (const auto& station: stations_) {
            if (station->is_ready()) { station->complete(); }
        }
        return true;
    }

    bool ForgeManager::is_ready_to_run()
    {
        if (disabled.get()) { return false; }
        static auto lambda = [](const std::unique_ptr<ForgeStation>& station) {
            return station->is_ready();
        };

        return std::ranges::any_of(stations_, lambda);
    }

    std::chrono::minutes ForgeManager::get_time_left_until_ready() const
    {
        return 0min;
    }
}

#include "cropmanager.h"

#include "../../common/util.h"
#include "../../config/config.h"


namespace llpp::bots::kitchen
{
    CropManager::CropManager()
    {
        using Type = CropStation::CropType;

        auto interval = std::chrono::hours(config::bots::crops::interval.get());
        for (int i = 0; i < config::bots::crops::num_longrass.get(); i++) {
            longrass_stations_.push_back(std::make_unique<CropStation>(
                util::add_num_to_prefix("LONGRASS", i + 1), Type::LONGRASS, interval));
        }

        for (int i = 0; i < config::bots::crops::num_citronal.get(); i++) {
            citronal_stations_.push_back(std::make_unique<CropStation>(
                util::add_num_to_prefix("CITRONAL", i + 1), Type::CITRONAL, interval));
        }

        for (int i = 0; i < config::bots::crops::num_rockarrot.get(); i++) {
            rockarrot_stations_.push_back(std::make_unique<CropStation>(
                util::add_num_to_prefix("ROCKARROT", i + 1), Type::ROCKARROT, interval));
        }

        for (int i = 0; i < config::bots::crops::num_savoroot.get(); i++) {
            savoroot_stations_.push_back(std::make_unique<CropStation>(
                util::add_num_to_prefix("SAVOROOT", i + 1), Type::SAVOROOT, interval));
        }
    }


    bool CropManager::run()
    {
        if (!is_ready_to_run() || config::bots::crops::disabled.get()) { return false; }

        for (const auto& station : longrass_stations_) { station->complete(); }
        for (const auto& station : citronal_stations_) { station->complete(); }
        for (const auto& station : rockarrot_stations_) { station->complete(); }
        for (const auto& station : savoroot_stations_) { station->complete(); }
    }

    bool CropManager::is_ready_to_run() const
    {
        return longrass_stations_[0]->is_ready();
    }

    std::chrono::minutes CropManager::get_time_left_until_ready() const
    {
        return util::get_time_left_until<std::chrono::minutes>(
            longrass_stations_[0]->get_next_completion());
    }
}

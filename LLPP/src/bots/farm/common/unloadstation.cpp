#include "unloadstation.h"

#include <asapp/core/state.h>

namespace llpp::bots::farm
{
    UnloadStation::UnloadStation(const std::string& t_name,
                                 std::shared_ptr<asa::entities::DinoEntity> t_anky)
        : TeleportStation(t_name, std::chrono::minutes(0)), anky_(std::move(t_anky)) {}

    core::StationResult UnloadStation::complete()
    {
        set_default_destination(true);
        if (!begin(false)) {
            set_completed();
            return {this, false, get_time_taken<>(), {}};
        }

        asa::core::sleep_for(5s);

        // The clicking is needed to be able to popcorn after searching,
        // ideally we would add some sort of flag for this to asapp.
        anky_->open_inventory();
        asa::core::sleep_for(1s);
        anky_->get_inventory()->search_bar.search_for("metal");
        asa::core::sleep_for(500ms);

        const auto pos = anky_->get_inventory()->slots[0].area.get_random_location(4);
        click_at(pos, asa::controls::LEFT);

        anky_->get_inventory()->toggle_tooltips();
        anky_->get_inventory()->popcorn_all();
        anky_->get_inventory()->toggle_tooltips();
        anky_->get_inventory()->close();
        return {this, true, get_time_taken<>(), {}};
    }
}

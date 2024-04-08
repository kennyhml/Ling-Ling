#include "mountstation.h"

#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/console.h>

namespace llpp::bots::farm
{
    MountStation::MountStation(const std::string& t_name,
                               std::shared_ptr<asa::entities::DinoEntity> t_anky,
                               std::unique_ptr<RenderStation> t_render_station,
                               std::unique_ptr<BedStation> t_reposition_bed)
        : BedStation(t_name, 0min), anky_(std::move(t_anky)),
          render_station_(std::move(t_render_station)),
          reposition_bed_(std::move(t_reposition_bed)) {}

    core::StationResult MountStation::complete()
    {
        if (render_station_) {
            if (!render_station_->complete().success) {
                return {this, false, get_time_taken<>(), {}};
            }
        }
        if (reposition_bed_) {
            whip_mount();
        }

        if (!begin(true)) {
            return {this, false, get_time_taken<>(), {}};
        }

        util::await([this] {
            return asa::entities::local_player->can_ride(*anky_);
        }, 30s);

        asa::entities::local_player->mount(*anky_);
        asa::entities::local_player->set_pitch(90);
        asa::entities::local_player->turn_left();
        return {this, true, get_time_taken<>(), {}};
    }

    void MountStation::whip_mount()
    {
        reposition_bed_->complete();
        if (!render_station_) {
            asa::core::sleep_for(5s);
            asa::interfaces::console->execute("reconnect");
            util::await([]() -> bool {
                return asa::entities::local_player->is_in_connect_screen();
            }, 1min);
            asa::core::sleep_for(10s);
        }

        const asa::structures::Container vault(get_name() + "::WHIP VAULT", 350);

        util::await(
            [&vault] { return asa::entities::local_player->can_access(vault); }, 60s);

        asa::entities::local_player->access(vault);
        vault.get_inventory()->take_slot(0);

        asa::entities::local_player->get_inventory()->select_slot(1);
        // Press "E" on the whip until it equips and the inventory automatically closes,
        // if the whip happens to be broken it will be repaired first granted dedis nearby
        // have the resources.
        while (vault.get_inventory()->is_open()) {
            asa::controls::press(asa::settings::use);
            asa::core::sleep_for(1s);
        }
        asa::core::sleep_for(1s);

        asa::entities::local_player->set_pitch(90);
        asa::core::sleep_for(500ms);
        asa::entities::local_player->primary_attack();
        asa::core::sleep_for(1s);

        asa::entities::local_player->set_pitch(0);
        asa::entities::local_player->access(vault);
        asa::entities::local_player->get_inventory()->transfer_all("whip");
        vault.get_inventory()->close();
    }
}

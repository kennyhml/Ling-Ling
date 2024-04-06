#include "forgestation.h"

#include <iostream>
#include <asapp/core/state.h>
#include <asapp/network/server.h>

#include "data.h"
#include "../../common/util.h"

namespace llpp::bots::forges
{
    namespace
    {
        std::array<asa::structures::Container, 3> create_forges(const std::string& prefix)
        {
            return {
                asa::structures::Container(prefix + "FORGE01", 100),
                asa::structures::Container(prefix + "FORGE02", 100),
                asa::structures::Container(prefix + "FORGE03", 100),
            };
        }
    }

    ForgeStation::ForgeStation(
        const std::string& t_name,
        const int32_t t_view_diff,
        std::shared_ptr<std::vector<LoadupStation> > t_loadup_stations,
        std::shared_ptr<std::vector<UnloadStation> > t_unload_stations)
        : BedStation(t_name + "::EMPTY", 5min),
          view_diff_(t_view_diff),
          fill_tp_(t_name + "::FILL", 0min),
          loadup_stations_(std::move(t_loadup_stations)),
          unload_stations_(std::move(t_unload_stations)),
          forges_(create_forges(t_name)) {}

    core::StationResult ForgeStation::complete()
    {
        bool at_unload = false;
        if (!is_empty() && has_finished_cooking()) {
            if (!empty_forges()) { return {this, false, get_time_taken<>(), {}}; }
            const std::string emptied_material = get_last_material();
            // Best to mark it as empty here already, even if its about to just get
            // filled with another material.
            set_cooking(get_name(), "");
            unload(emptied_material);
            at_unload = true;
        }
        if (std::string material; loadup(at_unload, material)) {
            fill_all();
            set_cooking(get_name(), material);
        }
        return {this, true, get_time_taken<>(), {}};
    }

    bool ForgeStation::is_empty() const
    {
        return get_last_material().empty();
    }

    bool ForgeStation::has_finished_cooking() const
    {
        return is_empty() || util::timedout(
                   std::chrono::system_clock::from_time_t(get_last_filled()),
                   get_current_material_cook_time());
    }

    bool ForgeStation::empty_forges()
    {
        // our station bed is the ::EMPTY bed right at the forges
        if (!begin(true)) { return false; }
        asa::entities::local_player->crouch();

        // The bed is facing the middle forge, so we need to empty one in front,
        // one on the left and one on the right.
        empty(forges_[1]);
        asa::entities::local_player->turn_left(90, 1s);
        empty(forges_[0]);
        asa::entities::local_player->turn_right(180, 1s);
        empty(forges_[2]);

        // Look straight and down, if the station is built correctly we should
        // be facing the bag we popcorned from the forge ahead.
        asa::entities::local_player->set_yaw(0);
        asa::entities::local_player->set_pitch(90);

        // Pressing F on this bag should collect the bags dropped from the other forges.
        // TODO: Send an error if we couldnt pick the items up & skip the unload.
        asa::core::sleep_for(1s);
        asa::entities::local_player->pick_up_all();
        asa::core::sleep_for(1s);
        return true;
    }

    void ForgeStation::fill_all()
    {
        // we cant be crouched because there a bed below the loadup teleporter.
        asa::entities::local_player->stand_up();
        fill_tp_.complete();

        asa::entities::local_player->crouch();
        asa::entities::local_player->set_pitch(0);
        asa::entities::local_player->set_yaw(-90);
        fill(forges_[0]);
        asa::entities::local_player->turn_right(90, 1s);
        fill(forges_[1]);
        asa::entities::local_player->turn_right(90, 1s);
        fill(forges_[2]);
    }

    void ForgeStation::unload(const std::string& material)
    {
        asa::entities::local_player->set_yaw(view_diff_);
        mediator_station_.set_default_destination(true);
        mediator_station_.complete();

        for (UnloadStation& station: *unload_stations_) {
            if (station.get_material() != material || !station.is_ready()) { continue; }

            std::cout << "Unloading at: " << station.get_name() << std::endl;
            station.set_default_destination(false);
            if (!station.complete().success) { throw std::exception("Unload failed"); }
            return;
        }
        throw std::exception("Could not find an unload station!");
    }

    bool ForgeStation::loadup(const bool is_at_unload, std::string& material_out)
    {
        if (!is_at_unload) {
            if (!begin(true)) { return false; }
            mediator_station_.set_default_destination(true);
            mediator_station_.complete();
            // Remember that, if we didnt empty the forges beforehand, we never did the
            // turn to compensate for the direction difference. Need to do it here.
            asa::entities::local_player->set_yaw(view_diff_);
        }
        // We are now either at the mediator tp or the unload tp, either way we can tp
        // as long as we are standing (the unload tp want to fast travel when crouched)
        asa::entities::local_player->stand_up();

        // Go through all loads up stations until we manage to get a slotcap of resources
        // to cook. If we cannot get a cap then we wont refill the station just yet.
        for (LoadupStation& station: *loadup_stations_) {
            if (!station.is_ready()) { continue; }
            station.set_default_destination(true);

            const auto result = station.complete();
            if (result.success && result.obtained_items.at(station.get_material()) > 0) {
                material_out = station.get_material();
                return true;
            }
        }
        // None of the loadup stations were able to get us a cap of items.
        return false;
    }

    void ForgeStation::empty(const asa::structures::Container& forge)
    {
        // TODO: Check for material mismatch between config and forge
        // TODO: Make sure everything is actually cooked.
        asa::entities::local_player->access(forge);
        forge.get_inventory()->popcorn_all();
        forge.get_inventory()->close();
        asa::core::sleep_for(1s);
    }

    void ForgeStation::fill(const asa::structures::Container& forge)
    {
        asa::entities::local_player->access(forge);
        asa::entities::local_player->get_inventory()->transfer_all();
        forge.get_inventory()->close();
        asa::core::sleep_for(1s);
    }

    std::string ForgeStation::get_last_material() const
    {
        std::string mat;
        int64_t _;
        get_cooking(get_name(), mat, _);
        return mat;
    }

    int64_t ForgeStation::get_last_filled() const
    {
        std::string _;
        int64_t last_filled;
        get_cooking(get_name(), _, last_filled);
        return last_filled;
    }

    std::chrono::minutes ForgeStation::get_current_material_cook_time() const
    {
        const std::string material = get_last_material();
        // Metal takes 4h 8min, so allow 4h 20min cause of lag & extra slot
        if (material == "METAL") { return 4h + 8min; }
        // Wood takes 2h 45min, so allow 15min longer to account for lag & the extra slot.
        if (material == "WOOD") { return 3h; }
        return 0min;
    }
}

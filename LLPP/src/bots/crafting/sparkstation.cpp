#include "sparkstation.h"
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include <asapp/items/items.h>

namespace llpp::bots::crafting
{
    SparkStation::SparkStation(const std::string& t_name,
                               const std::chrono::minutes t_interval) :
        BaseStation(t_name, t_interval), chem_bench_(name_ + "CHEMBENCH", 100),
        bed_(name_) {}

    core::StationResult SparkStation::complete()
    {
        asa::entities::local_player->fast_travel_to(bed_);
        const auto start = std::chrono::system_clock::now();

        if (state_ == CRAFTING) {
            empty_chembench();
            state_ = WAITING;
        }

        queue_spark();
        state_ = CRAFTING;

        auto time_taken = util::get_elapsed<std::chrono::seconds>(start);
        set_completed();
        return {this, true, time_taken, {}};
    }

    void SparkStation::empty_chembench()
    {
        asa::entities::local_player->access(chem_bench_);
        chem_bench_.get_inventory()->transfer_all();
        chem_bench_.get_inventory()->close();

        asa::core::sleep_for(std::chrono::seconds(1));

        asa::entities::local_player->set_yaw(180);

        asa::entities::local_player->crouch();
        asa::window::press(asa::settings::use);
        asa::core::sleep_for(std::chrono::milliseconds(500));

        asa::entities::local_player->stand_up();
        asa::window::press(asa::settings::use);
        asa::core::sleep_for(std::chrono::milliseconds(500));

        asa::entities::local_player->turn_up(30);
        asa::window::press(asa::settings::use);
        asa::core::sleep_for(std::chrono::milliseconds(500));

        asa::entities::local_player->turn_down(30);
        asa::entities::local_player->set_yaw(0);
    }

    void SparkStation::queue_spark()
    {
        asa::entities::local_player->access(chem_bench_);
        chem_bench_.get_inventory()->craft(*asa::items::resources::sparkpowder, 800);
        chem_bench_.get_inventory()->close();
        asa::core::sleep_for(std::chrono::seconds(1));

    }
}

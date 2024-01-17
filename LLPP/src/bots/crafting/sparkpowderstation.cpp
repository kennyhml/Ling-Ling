#include "sparkpowderstation.h"
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include <asapp/items/items.h>

#include "embeds.h"
#include "../../common/util.h"
#include "asapp/util/util.h"

namespace llpp::bots::crafting
{
    SparkpowderStation::SparkpowderStation(const std::string& t_name,
                                           const std::chrono::minutes t_interval) :
        BaseStation(t_name, t_interval), chem_bench_(name_ + "CHEMBENCH", 100),
        bed_(name_) {}

    core::StationResult SparkpowderStation::complete()
    {
        asa::entities::local_player->fast_travel_to(bed_);
        const auto start = std::chrono::system_clock::now();

        if (state_ == CRAFTING) {
            empty_chembench();
            state_ = WAITING;
        }

        queue_spark();
        state_ = CRAFTING;

        const auto time_taken = util::get_elapsed<std::chrono::seconds>(start);
        set_completed();
        const core::StationResult res(this, true, time_taken, {});
        send_spark_crafted(res);
        return res;
    }

    void SparkpowderStation::empty_chembench()
    {
        asa::entities::local_player->access(chem_bench_);
        chem_bench_.get_inventory()->transfer_all();
        chem_bench_.get_inventory()->close();

        asa::core::sleep_for(std::chrono::seconds(1));
        asa::entities::local_player->set_yaw(180);

        asa::entities::local_player->turn_down(20);
        asa::entities::local_player->crouch();

        for (int i = 0; i < 3; i++) {
            asa::core::sleep_for(std::chrono::milliseconds(500));
            asa::window::press(asa::settings::use);

            if (i == 0) { asa::entities::local_player->stand_up(); }
            else if (i == 1) { asa::entities::local_player->turn_up(50); }
        }

        asa::entities::local_player->turn_down(30);
        asa::core::sleep_for(std::chrono::seconds(1));
        const cv::Mat dedis = asa::window::screenshot();

        asa::entities::local_player->get_inventory()->open();

        if (!util::await([]() {
            return asa::entities::local_player->get_inventory()->slots[1].is_empty();
        }, std::chrono::seconds(10))) { send_station_capped(name_, dedis); }
        asa::entities::local_player->get_inventory()->close();
        asa::core::sleep_for(std::chrono::milliseconds(500));
        asa::entities::local_player->set_yaw(0);
    }

    void SparkpowderStation::queue_spark()
    {
        asa::entities::local_player->access(chem_bench_);
        chem_bench_.get_inventory()->craft(*asa::items::resources::sparkpowder, 800);
        chem_bench_.get_inventory()->close();
        asa::core::sleep_for(std::chrono::seconds(2));
    }
}

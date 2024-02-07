#include "forgestation.h"
#include <asapp/entities/localplayer.h>
#include <asapp/core/state.h>
#include <asapp/items/items.h>
#include "embeds.h"
#include <asapp/interfaces/tribemanager.h>
#include "../../core/discord.h"

namespace llpp::bots::crafting
{
    ForgeStation::ForgeStation(std::string t_name, const std::chrono::minutes t_interval)
            : BaseStation(std::move(t_name), t_interval), dedi_raw_(name_ + "::RAW DEDI"),
              dedi_cooked_(name_ + "COOKED DEDI"), forge_(name_ + "FORGE", 100),
              bed_(name_) {}


    core::StationResult ForgeStation::complete()
    {
        asa::entities::local_player->fast_travel_to(bed_, AccessFlags_Default,
                                                    TravelFlags_NoTravelAnimation);
        asa::interfaces::tribe_manager->update_tribelogs(core::discord::handle_tribelogs);
        asa::core::sleep_for(std::chrono::seconds(1));

        const auto start = std::chrono::system_clock::now();

        empty();
        if (!last_known_item_) {
            set_completed();
            return {this, false, std::chrono::seconds(0), {}};
        }
        fill();
        set_completed();

        const auto time_taken = util::get_elapsed<std::chrono::seconds>(start);

        if (!last_known_item_) {
            return {this, false, time_taken, {}};
        }
        auto item = last_known_item_->get_name();
        core::StationResult res(this, true, time_taken, {{item, last_amount_taken_}});
        send_forge_cycled(res, item, last_amount_added_);
        return res;
    }

    void ForgeStation::empty()
    {
        asa::entities::local_player->set_pitch(-90);
        asa::core::sleep_for(std::chrono::seconds(1));
        asa::entities::local_player->access(forge_);
        asa::core::sleep_for(std::chrono::seconds(1));

        const int slots_before = forge_.get_current_slots();
        // empty forge, nothing to really do here.
        if (slots_before == 0) {
            forge_.get_inventory()->close();
            asa::core::sleep_for(std::chrono::seconds(1));
            asa::entities::local_player->set_pitch(0);
            return;
        }

        last_known_item_ = nullptr;

        if (forge_.get_inventory()->has(*asa::items::resources::charcoal, true)) {
            last_known_item_ = asa::items::resources::charcoal;
        } else {
            forge_.get_inventory()->search_bar.delete_search();
            if (forge_.get_inventory()->has(*asa::items::resources::metal_ingot, true)) {
                last_known_item_ = asa::items::resources::metal_ingot;
            }
        }
        if (!last_known_item_) {
            forge_.get_inventory()->close();
            return;
        }
        forge_.get_inventory()->transfer_all();

        // wait a little for the server
        auto& slot = asa::entities::local_player->get_inventory()->slots[1];
        util::await([&slot]() -> bool { return !slot.is_empty(); },
                    std::chrono::seconds(10));

        const int slots_after = forge_.get_current_slots();
        forge_.get_inventory()->close();
        asa::core::sleep_for(std::chrono::seconds(1));

        asa::entities::local_player->crouch();
        asa::entities::local_player->set_pitch(10);
        asa::core::sleep_for(std::chrono::milliseconds(500));
        asa::entities::local_player->deposit_into_dedi(*last_known_item_, nullptr);
        asa::entities::local_player->stand_up();
        asa::entities::local_player->set_pitch(0);

        last_amount_taken_ =
                (slots_before - slots_after) * last_known_item_->get_data().stack_size;
    }

    void ForgeStation::fill()
    {
        asa::entities::local_player->access(dedi_raw_);

        dedi_raw_.get_inventory()->transfer_all();

        // TODO: Check if we got any items, if not the station is out of stuff to cook.
        dedi_raw_.get_inventory()->close();
        asa::core::sleep_for(std::chrono::seconds(1));

        asa::entities::local_player->set_pitch(-90);
        asa::entities::local_player->access(forge_);
        asa::core::sleep_for(std::chrono::seconds(1));
        const int slots_before = forge_.get_current_slots();

        asa::entities::local_player->get_inventory()->transfer_all();
        asa::core::sleep_for(std::chrono::seconds(1));
        const int slots_after = forge_.get_current_slots();

        last_amount_added_ =
                (slots_after - slots_before) * last_known_item_->get_data().stack_size;

        forge_.get_inventory()->close();
        asa::core::sleep_for(std::chrono::seconds(1));
        asa::entities::local_player->set_pitch(0);
        asa::core::sleep_for(std::chrono::milliseconds(500));
        asa::window::press(asa::settings::use);
        asa::core::sleep_for(std::chrono::milliseconds(500));
    }


}
#include "element_deposit.h"
#include "../../../common/util.h"
#include <asapp/interfaces/hud.h>
#include <asapp/entities/localplayer.h>
#include <asapp/core/state.h>
#include <asapp/entities/exceptions.h>

namespace llpp::bots::boss
{
    namespace
    {
        asa::entities::DinoEntity generic_rex("BOSS REX");

        bool check_can_sit()
        {
            asa::entities::local_player->go_back(3s);
            asa::entities::local_player->go_right(3s);
            asa::entities::local_player->go_forward(3s);
            asa::entities::local_player->go_left(3s);

            asa::entities::local_player->set_pitch(45);

            for (int i = 0; i < 360; i += 10) {
                asa::entities::local_player->turn_right(10);
                if (util::await(
                    [] { return asa::entities::local_player->can_sit_down(); }, 100ms)) {
                    return true;
                }
            }
            return false;
        }

        bool chair_to_element_dedi()
        {
            const auto start = std::chrono::system_clock::now();

            while (!asa::interfaces::hud->can_deposit()) {
                asa::controls::press(asa::settings::use);
                asa::core::sleep_for(1s);

                if (util::timedout(start, 1min)) { return false; }
            }

            asa::controls::press(asa::settings::use);
            asa::entities::local_player->crouch();
            asa::core::sleep_for(1s);
            asa::controls::press(asa::settings::use);
            return true;
        }
    }

    DepositResult deposit_element()
    {
        const auto start = std::chrono::system_clock::now();

        asa::entities::local_player->reset_pitch();
        asa::entities::local_player->turn_down(90);
        asa::entities::local_player->reset_pitch();
        asa::entities::local_player->turn_down(90);
        asa::entities::local_player->set_pitch(0);

        if (!util::await(
            [] { return asa::entities::local_player->is_in_travel_screen(); }, 2min)) {
            throw std::exception("Bossfight not left");
        }
        if (!util::await([] {
            return !asa::entities::local_player->is_in_travel_screen();
        }, 20s)) {
            throw std::exception("Bossfight loading screen not passed!");
        }

        asa::core::sleep_for(5s);

        if (check_can_sit()) {
            const bool put_into_dedi = chair_to_element_dedi();
            return {put_into_dedi, true, util::get_elapsed<std::chrono::seconds>(start)};
        }

        bool any_accessed = false;
        for (int i = 0; i < 4; i++) {
            try {
                asa::entities::local_player->access(generic_rex, 15s);
                any_accessed = true;
                break;
            } catch (const asa::entities::EntityNotAccessed& e) {}
            asa::entities::local_player->turn_right(90);
        }
        if (any_accessed) {
            asa::entities::local_player->get_inventory()->transfer_all("element");
            generic_rex.get_inventory()->close();
            return {true, false, util::get_elapsed<std::chrono::seconds>(start)};
        }
        // we couldnt put the ele into a rex or dedi
        return {false, false, util::get_elapsed<std::chrono::seconds>(start)};
    }
}

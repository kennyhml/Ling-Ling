#include "brood.h"
#include "embeds.h"
#include "../common/element_deposit.h"
#include "../../../discord/embeds.h"
#include "../../../config/config.h"
#include "../../../common/util.h"
#include "../../../core/bedstation.h"
#include "../../../core/teleportstation.h"
#include "../../../discord/bot.h"
#include "../../common/artifactstation.h"
#include "../common/crate_collect.h"
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/hud.h>
#include <asapp/structures/tributeable.h>
#include <iostream>
#include <asapp/core/state.h>
#include <asapp/entities/exceptions.h>


namespace llpp::bots::boss
{
    using namespace config::bots::boss;
    using asa::interfaces::PlayerInfo;

    namespace
    {
        core::BedStation trans_bed("BOSS::TRANS", 5min);
        core::BedStation whistle_bed("BOSS::WHISTLE", 5min);

        core::TeleportStation trans_tp("BOSS::TRANS", 0min);

        std::array artifact_stations{
            ArtifactStation{"HUNTER ARTIFACT", 0min},
            ArtifactStation{"CLEVER ARTIFACT", 0min},
            ArtifactStation{"MASSIVE ARTIFACT", 0min}
        };

        asa::structures::Tributable trans("BROOD TRANSMITTER", 0);

        /**
         * @brief Checks if the brood is ready, we run the brood in a 20min interval
         * as that should be the average time it will take the rexes to heal the damage
         * they take (~2.5K dmg per fight).
         */
        bool is_ready()
        {
            return util::timedout(
                       std::chrono::system_clock::from_time_t(last_brood.get()), 20min)
                   && trans_bed.is_ready();
        }

        void collect_required_artifacts()
        {
            for (auto& station: artifact_stations) {
                const auto result = station.complete();
                if (!result.success) { return; }

                if (result.obtained_items.at("Artifact") == 0) {
                    // Send an error to discord informing that we didnt get the artifact.
                    // There should be spare sets so its okay, but still good to know.l
                }
            }
            trans_tp.complete();
        }

        void equip_flak()
        {
            asa::core::sleep_for(500ms);
            asa::entities::local_player->get_inventory()->open();
            asa::entities::local_player->get_inventory()->toggle_tooltips();

            util::await([] {
                return !asa::entities::local_player->get_inventory()->slots[1].is_empty();
            }, 10s);

            asa::entities::local_player->get_inventory()->search_bar.search_for("flak");
            asa::entities::local_player->get_inventory()->select_info_tab();
            while (!asa::entities::local_player->get_inventory()->slots[0].is_empty()) {
                for (int i = 4; i >= 0; i--) {
                    asa::entities::local_player->get_inventory()->select_slot(i, false);
                    asa::controls::press(asa::settings::use);
                }
                asa::core::sleep_for(1s);
            }
            asa::entities::local_player->get_inventory()->toggle_tooltips();
            asa::entities::local_player->get_inventory()->close();
            asa::core::sleep_for(500ms);
        }

        void start_boss_fight()
        {
            asa::entities::local_player->stand_up();
            asa::entities::local_player->set_pitch(-90);
            asa::entities::local_player->access(trans);
            asa::entities::local_player->get_inventory()->transfer_all("artifact");
            util::await([] {
                return asa::entities::local_player->get_inventory()->slots[1].is_empty();
            }, 10s);

            trans.get_inventory()->summon("gamma broodmother");
            trans.get_inventory()->close();
            asa::core::sleep_for(1s);
            asa::entities::local_player->set_pitch(0);
            asa::entities::local_player->turn_left(20);
        }

        void enter_boss_fight()
        {
            util::await([] { return asa::entities::local_player->is_in_travel_screen(); },
                        40s);
            std::cout << "Entering arena..." << std::endl;

            util::await(
                [] { return !asa::entities::local_player->is_in_travel_screen(); }, 10s);
            std::cout << "BOSS HAS BEGUN!" << std::endl;
        }

        void complete_bossfight()
        {
            const auto start = std::chrono::system_clock::now();
            asa::controls::press(asa::settings::aggressive);
            asa::controls::key_down("s");

            for (int i = 0; i < 3; i++) {
                asa::controls::press(asa::settings::move_to);
                asa::core::sleep_for(1500ms);
            }

            while (!util::timedout(start, 15s)) {}
            asa::controls::key_up("s");

            asa::entities::local_player->prone();
            std::cout << "NOW WAITING FOR THE FIGHT TO FINISH" << std::endl;

            while (!asa::interfaces::HUD::item_added({21, 445, 113, 301})) {}

            std::cout << "BROOD DEFEATED!" << std::endl;
            asa::entities::local_player->stand_up();
            asa::core::sleep_for(2s);
        }
    }

    bool run_brood_if_ready()
    {
        if (!is_ready() || disabled.get()) { return false; }
        if (!trans_bed.complete().success) { return false; }

        const auto start = std::chrono::system_clock::now();

        collect_required_artifacts();
        start_boss_fight();
        last_brood.set(util::time_t_now());

        enter_boss_fight();
        const auto fight_start = std::chrono::system_clock::now();
        complete_bossfight();
        const auto fight_time = util::get_elapsed<std::chrono::seconds>(fight_start);

        // Don't bother trying to deposit element if we werent able to loot it in the first
        // place. This shouldnt happen often at all but it's a worthwhile optimization.
        cv::Mat ele_image;
        const bool element_obtained = collect_element_crate(CrateTier::GAMMA, ele_image);
        bool ele_in_dedi = false;

        if (element_obtained) {
            auto [success, in_dedi, _] = deposit_element();
            if (!success) {
                discord::get_bot()->message_create(
                    discord::create_error_message("Failed to deposit the Element."));
                asa::entities::local_player->suicide();
                return true;
            }
            ele_in_dedi = in_dedi;
        }

        const auto total_time = util::get_elapsed<std::chrono::seconds>(start);
        const dpp::message msg = get_brood_defeated_message(
            fight_time, total_time, ele_image, element_obtained, ele_in_dedi);
        discord::get_bot()->message_create(msg);
        return true;
    }
}

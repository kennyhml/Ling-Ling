#include "dropoffstation.h"

#include <iostream>
#include <asapp/core/state.h>
#include <asapp/items/items.h>

namespace llpp::bots::drops
{
    namespace
    {
        std::map<std::string, std::function<void()> > dedi_views{
            {
                "C4 Charge", [] {
                    asa::entities::local_player->crouch();
                    asa::entities::local_player->set_pitch(20);
                    asa::entities::local_player->set_yaw(160);
                }
            },
            {
                "Metal Arrow", [] {
                    asa::entities::local_player->crouch();
                    asa::entities::local_player->set_pitch(0);
                    asa::entities::local_player->set_yaw(160);
                }
            },
            {
                "Simple Shotgun Ammo", [] {
                    asa::entities::local_player->stand_up();
                    asa::entities::local_player->set_pitch(-10);
                    asa::entities::local_player->set_yaw(160);
                }
            },
            {
                "Rocket Propelled Grenade", [] {
                    asa::entities::local_player->crouch();
                    asa::entities::local_player->set_pitch(20);
                    asa::entities::local_player->set_yaw(-160);
                }
            },
            {
                "Spear Bolt", [] {
                    asa::entities::local_player->crouch();
                    asa::entities::local_player->set_pitch(0);
                    asa::entities::local_player->set_yaw(-160);
                }
            },
            {
                "Advanced Sniper Bullet", [] {
                    asa::entities::local_player->stand_up();
                    asa::entities::local_player->set_pitch(-10);
                    asa::entities::local_player->set_yaw(-160);
                }
            },
            {
                "Stone Arrow", [] {
                    asa::entities::local_player->crouch();
                    asa::entities::local_player->set_pitch(20);
                    asa::entities::local_player->set_yaw(-130);
                }
            },
            {
                "Simple Bullet", [] {
                    asa::entities::local_player->crouch();
                    asa::entities::local_player->set_pitch(0);
                    asa::entities::local_player->set_yaw(-130);
                }
            },
            {
                "Advanced Rifle Bullet", [] {
                    asa::entities::local_player->stand_up();
                    asa::entities::local_player->set_pitch(-10);
                    asa::entities::local_player->set_yaw(-130);
                }
            }
        };

        std::array<asa::structures::Container, 3> make_vaults()
        {
            return {
                asa::structures::Container("BLUEPRINTS VAULT", 350),
                asa::structures::Container("SADDLES VAULT", 350),
                asa::structures::Container("GEAR VAULT", 350)
            };
        }

        bool has_any_blueprints(const std::vector<LootResult>& items)
        {
            return std::ranges::any_of(items, [](const LootResult& item) -> bool {
                return item.looted && item.name.find("Blueprint") != std::string::npos;
            });
        }

        bool has_any_saddles(const std::vector<LootResult>& items)
        {
            return std::ranges::any_of(items, [](const LootResult& item) -> bool {
                return item.looted && item.name.find("Saddle") != std::string::npos
                       && item.name.find("Blueprint") == std::string::npos;
            });
        }

        bool has_any_consumables(const std::vector<LootResult>& items)
        {
            for (const auto& [name, looted, tooltip]: items) {
                if (!looted) { continue; }
                for (const auto& consumable: asa::items::consumables::iter()) {
                    if (name == (*consumable)->get_name()) { return true; }
                }
            }
            return false;
        }
    }

    DropoffStation::DropoffStation()
        : TeleportStation("DROPS::DROPOFF", 0min), align_bed_("ALIGN BED"),
          fridge_("CONSUMABLES", 80), vaults_(make_vaults()) {}

    core::StationResult DropoffStation::complete()
    {
        if (!begin(true)) {
            return {this, false, get_time_taken<>(), {}};
        }

        asa::entities::local_player->stand_up();
        for (int i = 0; i < 3; i++) {
            try {
                asa::entities::local_player->lay_on(align_bed_, AccessFlags_AccessAbove);
                break;
            } catch (const std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
        }
        asa::core::sleep_for(5s);
        asa::controls::press(asa::settings::use);
        asa::core::sleep_for(2s);

        asa::entities::local_player->reset_state();

        if (has_any_consumables(to_sort_)) { deposit_into_fridge(); }
        if (has_any_blueprints(to_sort_)) { sort_into_vault(VaultType::BLUEPRINTS); }
        if (has_any_saddles(to_sort_)) { sort_into_vault(VaultType::SADDLES); }

        for (const auto& [name, looted, tooltip]: to_sort_) {
            if (!looted) { continue; }
            sort_into_dedi(name);
        }
        sort_into_vault(VaultType::GRINDABLES);
        return {this, true, get_time_taken<>(), {}};
    }

    void DropoffStation::sort_into_vault(const VaultType vault)
    {
        asa::entities::local_player->stand_up();
        asa::entities::local_player->set_pitch(0);

        switch (vault) {
            case VaultType::BLUEPRINTS: {
                asa::entities::local_player->set_yaw(-40);
                asa::core::sleep_for(500ms);
                asa::entities::local_player->access(vaults_[0]);
                asa::entities::local_player->get_inventory()->transfer_all("blueprint");
                vaults_[0].get_inventory()->close();
                break;
            }
            case VaultType::SADDLES: {
                asa::entities::local_player->set_yaw(0);
                asa::core::sleep_for(500ms);
                asa::entities::local_player->access(vaults_[1]);
                asa::entities::local_player->get_inventory()->transfer_all("saddle");
                vaults_[1].get_inventory()->close();
                break;
            }
            case VaultType::GRINDABLES: {
                asa::entities::local_player->set_yaw(40);
                asa::core::sleep_for(500ms);
                asa::entities::local_player->access(vaults_[2]);
                asa::entities::local_player->get_inventory()->transfer_all();
                vaults_[2].get_inventory()->close();
                break;
            }
        }
        asa::core::sleep_for(1s);
    }

    void DropoffStation::deposit_into_fridge()
    {
        asa::entities::local_player->stand_up();

        asa::entities::local_player->set_pitch(0);
        asa::entities::local_player->set_yaw(-80);

        asa::entities::local_player->access(fridge_);
        asa::entities::local_player->get_inventory()->transfer_all();
        fridge_.get_inventory()->close();
        asa::core::sleep_for(1s);
    }

    void DropoffStation::sort_into_dedi(const std::string& item)
    {
        if (!dedi_views.contains(item)) { return; }
        dedi_views.at(item)();

        asa::core::sleep_for(500ms);
        asa::controls::press(asa::settings::use);
        asa::core::sleep_for(500ms);
    }
}

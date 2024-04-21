#include "taskmanager.h"

#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/spawnmap.h>

#include "../bots/drops/cratemanager.h"
#include "../bots/farm/swingbot/farmbot.h"
#include "../bots/kitchen/cropmanager.h"
#include "../bots/kitchen/sapmanager.h"
#include "../bots/paste/pastemanager.h"
#include "../bots/crafting/craftingmanager.h"
#include "../bots/common/suicidestation.h"
#include "../config/config.h"
#include "../bots/parasaur/parasaurmanager.h"
#include "../bots/farm/metal/metalmanager.h"
#include "../bots/farm/obsidian/obsidianmanager.h"
#include "../bots/farm/wood/woodmanager.h"
#include "../bots/forges/forgemanager.h"

namespace llpp::core
{
    namespace
    {
        auto last_inv_check = std::chrono::system_clock::now();
        std::chrono::seconds inv_check_interval(60);

        bots::SuicideStation suicide("SUICIDE DEATH", "SUICIDE RESPAWN");

        bool player_requires_healing()
        {
            static asa::structures::SimpleBed generic_bed("generic bed");

            // Player is currently dead.
            if (asa::interfaces::spawn_map->is_open()) { return false; }

            const auto& p = asa::entities::local_player;
            if (p->is_out_of_food() || p->is_out_of_water() || p->is_broken_bones()) {
                return true;
            }
            if (!util::timedout(last_inv_check, inv_check_interval)) { return false; }

            if (generic_bed.get_interface()->is_open()) {
                generic_bed.get_interface()->close();
            }

            auto info = p->get_inventory()->get_info();
            p->get_inventory()->open();
            const bool ret = info->get_health_level() < 0.7f || info->get_water_level() <
                             0.5f || info->get_food_level() < 0.5f;
            asa::entities::local_player->get_inventory()->close();
            asa::core::sleep_for(std::chrono::seconds(1));
            last_inv_check = std::chrono::system_clock::now();
            return ret;
        }

        bool player_state_check()
        {
            if (player_requires_healing()) { return suicide.complete().success; }
            return false;
        }
    }

    void TaskManager::collect_tasks()
    {
        using namespace llpp::bots;

        tasks_.emplace_back("STATE CHECK", player_state_check);
        tasks_.emplace_back("FARMING", farm::run_while_requested);

        tasks_.emplace_back("PARASAURS", std::make_unique<parasaur::ParasaurManager>());
        tasks_.emplace_back("CRAFTING", std::make_unique<crafting::CraftingManager>());

        for (auto& manager: farm::create_metal_managers()) {
            tasks_.emplace_back("", std::move(manager));
        }
        for (auto& manager: farm::create_wood_managers()) {
            tasks_.emplace_back("", std::move(manager));
        }
        for (auto& manager: farm::create_obsidian_managers()) {
            tasks_.emplace_back("", std::move(manager));
        }

        tasks_.emplace_back("FORGES", std::make_unique<forges::ForgeManager>());

        for (auto& manager: drops::create_managers()) {
            tasks_.emplace_back("", std::move(manager));
        }

        tasks_.emplace_back("PASTE", std::make_unique<paste::PasteManager>());
        tasks_.emplace_back("CROPS", std::make_unique<kitchen::CropManager>());
        tasks_.emplace_back("SAP", std::make_unique<kitchen::SapManager>());
        has_collected_tasks_ = true;
    }

    const Task* TaskManager::get_previous_task() const
    {
        return &tasks_[(task_ptr_ - 1) % tasks_.size()];
    }

    const Task* TaskManager::get_current_task() const
    {
        return &tasks_[task_ptr_ % tasks_.size()];
    }

    const Task* TaskManager::get_next_task() const
    {
        return &tasks_[(task_ptr_ + 1) % tasks_.size()];
    }

    void TaskManager::execute_next()
    {
        const Task* task = nullptr;

        while (!task || !task->execute()) {
            task = &tasks_[task_ptr_++];
            task_ptr_ %= tasks_.size();
        }
        task_ptr_ = 0;
    }
}

#include "taskmanager.h"

#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/hud.h>

#include "../bots/drops/cratemanager.h"
#include "../bots/kitchen/cropmanager.h"
#include "../bots/kitchen/sapmanager.h"
#include "../bots/paste/pastemanager.h"
#include "../bots/suicide/suicidestation.h"
#include "../config/config.h"


namespace llpp::core
{
    namespace
    {
        bool player_state_check()
        {
            static bots::suicide::SuicideStation suicide(
                "SUICIDE DEATH", "SUICIDE RESPAWN");
            auto& player = asa::entities::local_player;

            bool need_suicide = false;
            // player->is_out_of_food() || player->is_out_of_water();
            if (!need_suicide) {
                auto& info = player->get_inventory()->info;
                player->get_inventory()->open();
                asa::core::sleep_for(std::chrono::seconds(3));
                need_suicide = info.get_health_level() < 0.75f || info.get_water_level() <
                    0.75f || info.get_food_level() < 0.75f;
                asa::entities::local_player->get_inventory()->close();
                asa::core::sleep_for(std::chrono::seconds(1));
            }

            if (need_suicide) { suicide.complete(); }
            return need_suicide;
        }
    }

    void TaskManager::collect_tasks()
    {
        assert(!has_collected_tasks(),
               "TaskManager::collect_tasks called twice on same instance.");
        using namespace llpp::bots;

        tasks_.emplace_back("STATE CHECK", player_state_check);
        tasks_.emplace_back("PASTE", std::make_unique<paste::PasteManager>());

        for (auto& [key, config] : config::bots::drops::configs) {
            tasks_.emplace_back(
                key, std::make_unique<drops::CrateManager>(*config.get_ptr()));
        }

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

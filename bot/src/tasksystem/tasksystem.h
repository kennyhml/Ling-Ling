#pragma once
#include "task.h"
#include "persistenttask.h"
#include "queue"
#include "config.h"
#include "discord.h"
#include "core/startup.h"

namespace lingling
{
    /**
     * @brief Initializes the task system, allows the components within the tasksystem to
     * register its callbacks during the early core application startup.
     */
    void init_tasksystem();

    // Runs the tasksystem init on module initialization
    inline run_on_startup tasksystem_load{startup_time::STARTUP_EARLY, init_tasksystem};
}

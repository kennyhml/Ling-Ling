#pragma once
#include "task.h"

#include <memory>

namespace lingling
{
    /**
     * @brief Gets the task that is currently being executed.
     *
     * @return A shared pointer to the task object that is being executed.
     */
    [[nodiscard]] std::shared_ptr<task> get_active_task();

    /**
     * @brief Executes the next task in the queue, the task to be executed will be set
     * as active and may be received through @link get_active_task \endlink.
     */
    void execute_next_task();

    /**
     * @brief Starts the thread responsible for executing tasks.
     */
    void start_task_executor_thread();

    /**
     * @brief Terminates the thread responsible for executing tasks.
     */
    void terminate_task_executor_thread();

    /**
     * @brief Gets the identifier of the task executor managed thread.
     */
    [[nodiscard]] std::string get_task_executor_thread_identifier();
}

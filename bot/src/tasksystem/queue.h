#pragma once
#include "task.h"

#include <deque>
#include <memory>

namespace lingling
{
    // Task queue (implemented as deque), upcoming tasks at front new tasks at back
    // When a task is inserted, it is inserted based on its priority, if other tasks
    // have the same priority, the inserted task is at the very end of them
    using task_queue_t = std::deque<std::shared_ptr<task> >;

    // Callback for modules to check if any of their tasks are ready to be enqueued
    // into the task list to be executed in the near future.
    using task_enqueue_lookup_callback_t = std::function<std::shared_ptr<task>()>;

    /**
     * @brief Gets the upcoming task and pops it from the queue.
     *
     * @return A shared pointer to the task to be executed.
     */
    [[nodiscard]] std::shared_ptr<task> get_next_task_in_queue();

    /**
     * @brief Gets the upcoming task in the queue without popping it, serves as view-only.
     *
     * @return A constant pointer to the upcoming task.
     */
    [[nodiscard]] const task* peek_next_task_in_queue();

    /**
     * @brief Gets the entire task queue without popping any of them.
     *
     * @return A constant reference to the current task queue.
     */
    const task_queue_t& get_all_tasks_in_queue();

    /**
     * @brief Adds the provided task to the task queue, it is inserted at the appropriate
     * position based on it's priority value.
     */
    void enqueue_task(const std::shared_ptr<task>& new_task);

    /**
     * @brief Erases the provided task from the task queue.
     */
    void dequeue_task(const std::shared_ptr<task>& erase_task);

    /**
    * @brief Registers a callback to be called when the task queue looks for a new
    * task to insert into the queue. The callback may return a task to enqueue or
    * nullptr in case it has no task available to push.
    */
    void register_task_enqueue_lookup_callback(task_enqueue_lookup_callback_t);

    /**
     * @brief Starts the queue handler thread responsible for managing the task queue, i.e
     * calling out to the modules to fetch any ready tasks to populate the queue with and
     * reacting to other changes within the queue.
     */
    void start_queue_handler_thread();
}

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

    // Callback for modules to check if any of their tasks are ready to be put into
    // the task queue
    using task_provider_t = std::function<std::vector<std::shared_ptr<task> >()>;

    // Callback for task queue being updated, a task was either changed, removed or
    // added.
    using task_queue_update_listener_t = std::function<void()>;

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
     * @return A copy of the current task queue.
     */
    task_queue_t get_all_tasks_in_queue();

    /**
     * @brief Adds the provided task to the task queue, it is inserted at the appropriate
     * position based on it's priority value.
     *
     * @param enqueue The Task to insert into the queue.
     * @param no_update If true, the queue update event will not be triggered.
     */
    void enqueue_task(const std::shared_ptr<task>& enqueue, bool no_update = false);

    /**
     * @brief Erases the provided task from the task queue.
     *
     * @param dequeue The Task to remove from the queue.
     * @param no_update If true, the queue update event will not be triggered.
     */
    void dequeue_task(const std::shared_ptr<task>& dequeue, bool no_update = false);

    /**
    * @brief Adds a listener to be called when the task queue looks for new tasks to
    * insert into the queue. The callback returns a list of all tasks to be enqueued.
    *
    * @param provider The function that will be called periodically and may return a
    * number of tasks to be enqueued.
    */
    void add_task_queue_task_provider(task_provider_t provider);

    /**
     * @brief Adds a listener to be called when the task queue is updated, the event
     * will trigger when a task was either changed, dequeued or enqueued.
     *
     * @param listener The function that will be called when the task queue changes.
     */
    void add_task_queue_updated_listener(task_queue_update_listener_t listener);

    /**
     * @brief Starts the queue handler thread responsible for managing the task queue, i.e
     * calling out to the modules to fetch any ready tasks to populate the queue with and
     * reacting to other changes within the queue.
     */
    void start_task_queue_manager_thread();

    /**
     * @brief Terminates the queue handler thead in a safe manner, once terminated the
     * attached task providers will no longer be called until the handler is restarted.
     *
     * @param clear Whether to clear the task queue of the remaining tasks.
     */
    void terminate_task_queue_manager_thread(bool clear = true);

    /**
     * @brief Gets the unique identifier of the task queue manager thread.
     */
    std::string get_task_queue_manager_thread_identifier();
}

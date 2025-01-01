#include "queue.h"

#include <asa/core/managedthread.h>
#include <asa/core/state.h>
#include <asa/core/logging.h>
#include <magic_enum.hpp>

namespace lingling
{
    namespace
    {
        constexpr auto TASK_READINESS_FETCH_INTERVAL = 5s;

        std::vector<task_enqueue_lookup_callback_t> callbacks;
        std::deque<std::shared_ptr<task> > task_queue;
        std::mutex queue_mutex;

    }

    std::shared_ptr<task> get_next_task_in_queue()
    {
        std::lock_guard lock(queue_mutex);
        if (task_queue.empty()) { return nullptr; }

        std::shared_ptr<task> ret = task_queue.front();
        task_queue.pop_front();
        return ret;
    }

    const task* peek_next_task_in_queue()
    {
        std::lock_guard lock(queue_mutex);
        if (task_queue.empty()) { return nullptr; }

        return task_queue.front().get();
    }

    task_queue_t get_all_tasks_in_queue()
    {
        std::lock_guard lock(queue_mutex);
        return task_queue;
    }

    void enqueue_task(const std::shared_ptr<task>& new_task)
    {
        asa::get_logger()->info("Enqueuing task '{}' ({})..", new_task->get_id(),
                                new_task->get_module());
        // Make sure the state of the task matches the fact that we are enqueuing it
        if (new_task->get_state() != task_state::STATE_WAITING) {
            asa::get_logger()->warn("Task state was not STATE_WAITING, enqueue skipped!");
            return;
        }
        std::lock_guard lock(queue_mutex);

        for (auto it = task_queue.begin(); it != task_queue.end(); ++it) {
            // Insert the new task before the first task with a lower or equal priority
            if ((*it)->get_priority() < new_task->get_priority()) {
                task_queue.insert(it, new_task);
                new_task->set_state(task_state::STATE_ENQUEUED);
                return;
            }
        }

        // Didnt insert the task then the queue is either empty or all tasks in the queue
        // had a higher priority, in either case it is fine to add it to the back.
        task_queue.emplace_back(new_task);
        new_task->set_state(task_state::STATE_ENQUEUED);
    }

    void dequeue_task(const std::shared_ptr<task>& erase_task)
    {
        std::lock_guard lock(queue_mutex);

        // Erase the task that matches the module and id of the provided task.
        std::erase_if(task_queue, [&erase_task](const std::shared_ptr<task>& t) {
            return t->get_module() == erase_task->get_module() &&
                   t->get_id() == erase_task->get_id();
        });
    }

    void add_task_enqueue_lookup_listener(task_enqueue_lookup_callback_t fn)
    {
        callbacks.push_back(std::move(fn));
    }

    void start_queue_handler_thread()
    {
        asa::register_thread("Taskqueue Handler", [] {
            // Go over the registered callbacks every 5 seconds.
            for (const auto& fn: callbacks) {
                if (std::shared_ptr<task> task = fn(); task != nullptr) {
                    enqueue_task(task);
                }
            }
            asa::checked_sleep(TASK_READINESS_FETCH_INTERVAL);
        });
        asa::get_thread("Taskqueue Handler")->start();
    }
}

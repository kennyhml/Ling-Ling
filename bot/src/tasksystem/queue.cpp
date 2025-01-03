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
        constexpr auto THREAD_IDENTIFIER = "Task Queue";

        std::vector<task_provider_t> task_providers;
        std::vector<task_queue_update_listener_t> update_listeners;

        std::deque<std::shared_ptr<task> > task_queue;
        std::mutex queue_mutex;

        /**
         * @brief Implementation of the periodic task fetch using the task providers.
         */
        void task_fetch_impl()
        {
            while (true) {
                for (const auto& provider: task_providers) {
                    const std::vector<std::shared_ptr<task> > tasks = provider();
                    if (tasks.empty()) { continue; }

                    for (const std::shared_ptr<task>& t: tasks) {
                        // dont update until last q'd
                        enqueue_task(t, t != tasks.back());
                    }
                }
                asa::checked_sleep(TASK_READINESS_FETCH_INTERVAL);
            }
        }

        /**
         * @brief Continously fetches the task providers for new tasks to enqueue into
         * the task queue. Handles the thread being interrupted either through a signal
         * or unhandled exception.
         */
        void continuous_task_fetch()
        {
            try {
                task_fetch_impl();
            } catch (const asa::thread_interruped& e) {
                asa::get_logger()->info(e.what());
            } catch (const std::exception& e) {
                asa::get_logger()->error("Error in Task Queue Manager: {}", e.what());
            }
        }

        // Informs all task queue update listeners that the task queue was changed.
        void dispatch_update_listeners()
        {
            for (const auto& fn: update_listeners) { fn(); }
        }
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

    void enqueue_task(const std::shared_ptr<task>& enqueue, const bool no_update)
    {
        asa::get_logger()->info(
            "Enqueuing task '{}' (Module: {})..", enqueue->get_id(), enqueue->get_module()
        );

        // Make sure the state of the task matches, this will also prevent registering
        // tasks twice unless their state changed in the meantime.
        if (enqueue->get_state() != task_state::STATE_QUEUEABLE) {
            asa::get_logger()->warn(
                "Task enqueue cancelled - state is '{}', expected STATE_QUEUABLE!",
                std::string(magic_enum::enum_name(enqueue->get_state())));
            return;
        }
        // Dont keep the queue mutex locked for the event dispatches.
        {
            std::lock_guard lock(queue_mutex);
            bool inserted = false;
            for (auto it = task_queue.begin(); it != task_queue.end(); ++it) {
                // Insert the new task before the first task with a lower priority
                if ((*it)->get_priority() < enqueue->get_priority()) {
                    task_queue.insert(it, enqueue);
                    inserted = true;
                    break;
                }
            }

            // Didnt insert the task then the queue is either empty or all tasks in the queue
            // had a higher priority, in either case it is fine to add it to the back.
            if (!inserted) { task_queue.emplace_back(enqueue); }
            enqueue->set_state(task_state::STATE_ENQUEUED);
        }
        if (!no_update) { dispatch_update_listeners(); }
    }

    void dequeue_task(const std::shared_ptr<task>& dequeue, const bool no_update)
    {
        // Dont keep the queue mutex locked for the event dispatches.
        {
            std::lock_guard lock(queue_mutex);

            // Erase the task that points to the same object in memory.
            std::erase_if(task_queue, [&dequeue](const std::shared_ptr<task>& t) {
                return t.get() == dequeue.get();
            });
        }
        if (!no_update) { dispatch_update_listeners(); }
    }

    void add_task_queue_task_provider(task_provider_t provider)
    {
        task_providers.push_back(std::move(provider));
    }

    void add_task_queue_updated_listener(task_queue_update_listener_t listener)
    {
        update_listeners.push_back(std::move(listener));
    }

    void start_task_queue_manager_thread()
    {
        // Dont need to add logging here since the called functions log plenty.
        asa::register_thread(THREAD_IDENTIFIER, continuous_task_fetch);
        asa::get_thread(THREAD_IDENTIFIER)->start();
    }

    void terminate_task_queue_manager_thread(const bool clear)
    {
        asa::get_thread(THREAD_IDENTIFIER)->set_state(asa::managed_thread::TERMINATED);
        if (clear) {
            task_queue.clear();
            dispatch_update_listeners();
        }
    }

    std::string get_task_queue_manager_thread_identifier()
    {
        return THREAD_IDENTIFIER;
    }
}

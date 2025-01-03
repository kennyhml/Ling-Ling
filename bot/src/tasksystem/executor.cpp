#include "executor.h"
#include "queue.h"

#include <asa/core/logging.h>
#include <asa/core/managedthread.h>
#include <asa/core/state.h>

namespace lingling
{
    namespace
    {
        constexpr auto EXECUTOR_THREAD_IDENTIFIER = "Task Runner";
        std::shared_ptr<task> active_task = nullptr;

        void executor_impl()
        {
            while (true) {
                try {
                    execute_next_task();
                    asa::checked_sleep(5ms);
                    // Checked sleep to allow pause / termination!
                } catch (const asa::thread_interruped& e) {
                    asa::get_logger()->info(e.what());
                    return;
                } catch (const std::exception& e) {
                    asa::get_logger()->error(e.what());
                }
            }
        }
    }

    std::shared_ptr<task> get_active_task() { return active_task; }

    void execute_next_task()
    {
        std::shared_ptr<task> next = get_next_task_in_queue();
        if (!next) {
            // Only log that there is no active task available if we previously had
            // one, that way it wont be logged over and over again.
            if (active_task) { asa::get_logger()->info("No task available."); }
            active_task = nullptr;
            return;
        }
        active_task = std::move(next);
        asa::get_logger()->info("Executing task: '{}' (Module: {})..",
                                active_task->get_id(), active_task->get_module());

        active_task->execute();
    }

    void start_task_executor_thread()
    {
        asa::register_thread(EXECUTOR_THREAD_IDENTIFIER, executor_impl);
        asa::get_thread(EXECUTOR_THREAD_IDENTIFIER)->start();
    }

    void terminate_task_executor_thread()
    {
        asa::get_thread(EXECUTOR_THREAD_IDENTIFIER)->set_state(
            asa::managed_thread::TERMINATED);
    }

    std::string get_task_executor_thread_identifier()
    {
        return EXECUTOR_THREAD_IDENTIFIER;
    }
}

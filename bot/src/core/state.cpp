#include "state.h"
#include "tasksystem/executor.h"
#include "tasksystem/queue.h"

#include <asa/core/logging.h>
#include <asa/utility.h>
#include <asa/core/managedthread.h>
#include <asa/core/state.h>
#include <magic_enum.hpp>

namespace lingling
{
    namespace
    {
        auto application_state = app_state::APP_STATE_ONLINE;

        /**
         * @brief Checks whether any managed thread is currently set to paused.
         */
        [[nodiscard]] bool is_any_thread_paused()
        {
            return std::ranges::any_of(asa::get_all_threads(), [](const auto& t) {
                return t.second->get_state() == asa::managed_thread::PAUSED;
            });
        }
    }

    void set_application_state(const app_state state)
    {
        const auto old = application_state;
        application_state = state;
        asa::get_logger()->info("Application state changed from '{}' to '{}'.",
                                magic_enum::enum_name(old), magic_enum::enum_name(state));
    }

    app_state get_application_state()
    {
        return application_state;
    }

    void start_application()
    {
        if (!asa::get_all_threads().empty()) {
            asa::get_logger()->error("Launch denied - one or more threads still active.");
            return;
        }
        asa::get_logger()->info("Launch requested. Starting required threads..");
        start_task_queue_manager_thread();
        start_task_executor_thread();
        set_application_state(app_state::APP_STATE_RUNNING);
    }

    void pause_application()
    {
        asa::get_logger()->info("Pause requested. Pausing all threads..");
        set_all_threads_state(asa::managed_thread::PAUSED);
        set_application_state(app_state::APP_STATE_PAUSED);
    }

    void resume_application()
    {
        asa::get_logger()->info("Resume requested. Resuming all threads..");
        set_all_threads_state(asa::managed_thread::RUNNING);
        set_application_state(app_state::APP_STATE_RUNNING);
    }

    void terminate_application()
    {
        if (asa::get_all_threads().empty()) {
            asa::get_logger()->error("Termination denied - no threads are active.");
            return;
        }
        asa::get_logger()->info("Termination requested. Interrupting all threads..");
        set_all_threads_state(asa::managed_thread::TERMINATED);
        set_application_state(app_state::APP_STATE_ONLINE);

        // Wait 5 seconds to see whether all threads have ended. During this time we also
        // wont be receiving any user input since it will block the thread. It is important
        // to call the checked sleep function here since that will pop threads that have
        // terminated from the thread registry.
        const asa::utility::stopwatch sw;
        while (!sw.timedout(5s)) {
            if (asa::get_all_threads().empty()) {
                asa::get_logger()->info("All threads terminated ({}ms elapsed).",
                                        sw.elapsed().count());
                return;
            }
            asa::checked_sleep(50ms);
        }
        asa::get_logger()->warn("One or more threads have not terminated within 5s.");
    }

    void check_for_user_keyboard_input_action()
    {
        static std::chrono::system_clock::time_point last_press;

        if (!asa::utility::timedout(last_press, 400ms)) return;
        if (GetAsyncKeyState(VK_F1) & 0x8000) {
            last_press = std::chrono::system_clock::now();
            start_application();
        } else if (GetAsyncKeyState(VK_F3) & 0x8000) {
            last_press = std::chrono::system_clock::now();
            terminate_application();
        } else if (GetAsyncKeyState(VK_F5) & 0x8000) {
            last_press = std::chrono::system_clock::now();
            if (asa::get_all_threads().empty()) {
                asa::get_logger()->error("Pause/Resume denied - no threads active.");
                return;
            }
            is_any_thread_paused() ? resume_application() : pause_application();
        }
        asa::checked_sleep(5ms);
    }
}

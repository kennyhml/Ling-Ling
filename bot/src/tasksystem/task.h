#pragma once
#include <chrono>
#include <string>

using namespace std::chrono_literals;

namespace lingling
{
    enum class task_priority
    {
        PRIORITY_DEFERRED, // on hold due to lack of urgency or dependency on other work
        PRIORITY_LOW,      // can be postponed indefinitely without significant impact
        PRIORITY_MEDIUM,   // necessary but not sensitive, may be delayed in moderation
        PRIORITY_HIGH,     // perform soon but dont pose a critical impact if not
        PRIORITY_CRITICAL  // performe immediately to avoid significant consequences
    };

    enum class task_state
    {
        TASK_ENABLED,  // The task is enabled and allowed to execute (default)
        TASK_DISABLED, // The task is disabled, it should not execute
        TASK_SUSPENDED // The task is temporarily suspended, it should not execute
    };

    class task
    {
    public:
        task(std::string t_id, std::string t_description, task_priority t_priority);

        /**
         * @brief Check whether the task is currently ready to be executed, this may be
         * due to many factors such as fixed timers or related tasks that need to be ready.
         *
         * @return Whether the task is currently ready to be executed.
         */
        virtual bool is_ready() = 0;

        /**
         * @brief Executed the task, should only be called if the task is also ready.
         *
         * @remark Upon completion, the
         */
        virtual void execute() = 0;

        /**
         * @brief Sets the task to the given priority, this may not take effect in the
         * task queue until the task was executed and is re-scheduled.
         *
         * @param priority The priority of the task, refer to @link task_priority \endlink.
         */
        virtual void set_priority(task_priority priority);

        /**
         * @brief Gets the current priority of the task. If needed, the priority may be
         * changed through @link set_priority \endlink.
         *
         * @return The current priority of the task.
         */
        [[nodiscard]] task_priority get_priority() const { return priority_; }

        /**
         * @brief Sets the task to the given state.
         *
         * @param state The state of the task, refer to @link task_state \endlink.
         */
        virtual void set_state(task_state state);

        /**
         * @brief Sets the task to the given state.
         *
         * @param state The state of the task, refer to @link task_state \endlink.
         * @param duration The suspension duration in case of TASK_SUSPENDED.
         */
        virtual void set_state(task_state state, std::chrono::minutes duration);

        /**
         * @brief Gets the current state of the task. If needed, the state may be
         * changed through @link set_state \endlink.
         *
         * @return The current priority of the task.
         */
        [[nodiscard]] task_state get_state() const { return state_; }

        virtual ~task() = default;

    private:
        std::string name_;
        std::string description_;

        task_priority priority_;
        task_state state_;

        std::chrono::system_clock::time_point last_completion_;
        std::chrono::system_clock::time_point suspension_start_;
    };
}

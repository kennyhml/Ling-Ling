#pragma once
#include <chrono>
#include <functional>
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

    struct task_result
    {
        // Whether the task was completed successfully
        bool succeeded = false;

        // The timestamp the task was started at
        std::chrono::system_clock::time_point start_time;

        // The timestamp the task was ended at (completed or failed)
        std::chrono::system_clock::time_point end_time;

        /**
         * @brief Gets the total amount of time taken for the task.
         */
        template<typename Duration = std::chrono::seconds>
        [[nodiscard]] Duration time_taken() const
        {
            return std::chrono::duration_cast<Duration>(end_time - start_time);
        }
    };

    using task_execution_callback_t = std::function<void(const task_result&)>;

    /**
     * @brief Abstract task base class that any task must implement in order to be
     * manageable through the taskmanager.
     */
    class task
    {
    public:
        task(std::string t_module, std::string t_id, std::string t_description,
             task_priority t_priority);

        /**
         * @brief Check whether the task is currently ready to be executed, this may be
         * due to many factors such as fixed timers or related tasks that need to be ready.
         *
         * @return Whether the task is currently ready to be executed.
         */
        [[nodiscard]] virtual bool is_ready() = 0;

        /**
         * @brief Executed the task, should only be called if the task is also ready.
         */
        virtual task_result& execute() = 0;

        /**
         * @brief Gets the module of this task, for example "gacha-tower" or "tribelogs".
         */
        [[nodiscard]] const std::string& get_module() const { return module_; }

        /**
         * @brief Gets the id of this task, for example "gacha-33-34".
         */
        [[nodiscard]] const std::string& get_id() const { return id_; }

        /**
         * @brief Gets the description of this task, for example "Feeds the Gacha".
         */
        [[nodiscard]] const std::string& get_description() const { return description_; }

        /**
         * @brief Registers a callback to be called when the task was executed. It is to
         * be called at the end of the task execution, regardless of failure or success.
         *
         * @remark As subclasses are unable to override the completion callback type, they
         * must cast the task result to the required concrete implementation.
         */
        void add_executed_listener(task_execution_callback_t callback);

        /**
         * @brief Sets the task to the given priority, this may not take effect in the
         * task queue until the task was executed and is re-scheduled.
         *
         * @param priority The priority of the task, refer to @link task_priority \endlink.
         */
        virtual void set_priority(const task_priority priority) { priority_ = priority; }

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

    protected:
        /**
         * @brief Triggers the execution listener callbacks with the last result.
         */
        void trigger_execution_callbacks() const;

        std::string module_;
        std::string id_;
        std::string description_;

        task_priority priority_;
        task_state state_ = task_state::TASK_ENABLED;
        task_result last_result_;

        std::chrono::system_clock::time_point last_completion_;
        std::chrono::system_clock::time_point suspension_start_;

        std::vector<task_execution_callback_t> callbacks_;
    };
}

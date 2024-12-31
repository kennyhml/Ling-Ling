#include "task.h"

namespace lingling
{
    task::task(std::string t_module, std::string t_id, std::string t_description,
               const task_priority t_priority)
        : module_(std::move(t_module)), id_(std::move(t_id)),
          description_(std::move(t_description)), priority_(t_priority) {}

    void task::add_executed_listener(task_execution_callback_t callback)
    {
        callbacks_.push_back(std::move(callback));
    }

    void task::set_state(const task_state state) { set_state(state, 30min); }

    void task::set_state(const task_state state, std::chrono::minutes duration)
    {
        state_ = state;
    }

    void task::trigger_execution_callbacks() const
    {
        for (const auto& fn: callbacks_) { fn(last_result_); }
    }
}

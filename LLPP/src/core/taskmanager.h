#pragma once
#include <vector>

#include "task.h"

namespace llpp::core
{
    class TaskManager
    {
    public:
        TaskManager() = default;

        void collect_tasks();
        void execute_next();

        [[nodiscard]] const Task* get_previous_task() const;
        [[nodiscard]] const Task* get_current_task() const;
        [[nodiscard]] const Task* get_next_task() const;

        [[nodiscard]] bool has_collected_tasks() const { return has_collected_tasks_; }

    private:
        int task_ptr_ = 0;
        bool has_collected_tasks_ = false;
        std::vector<Task> tasks_;
    };
}

#include "task.h"

namespace llpp::core
{
    Task::Task(std::string t_name, std::function<void()> t_run) :
        name_(std::move(t_name)), manager_(nullptr), run_(std::move(t_run)) {}

    Task::Task(std::string t_name, std::unique_ptr<IStationManager> t_station_manager) :
        name_(std::move(t_name)), manager_(std::move(t_station_manager)), run_(nullptr) {}

    bool Task::execute() const
    {
        if (run_) {
            run_();
            return true;
        }
        return manager_->run();
    }
}
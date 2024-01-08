#pragma once
#include "istationmanager.h"
#include <memory>
#include <functional>

namespace llpp::core
{
    class Task
    {
    public:
        explicit Task(std::string t_name,
                      std::unique_ptr<IStationManager> t_station_manager);
        explicit Task(std::string t_name, std::function<void()> t_run);

        bool execute() const;
        const std::string& get_name() const { return name_; }

    private:
        std::string name_;
        std::unique_ptr<IStationManager> manager_;
        std::function<void()> run_;
    };
}

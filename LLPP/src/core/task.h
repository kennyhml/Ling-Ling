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
        explicit Task(std::string t_name, std::function<bool()> t_run);

        [[nodiscard]] bool execute() const;
        [[nodiscard]] bool requires_full_health() const { return requires_full_hp_; }
        [[nodiscard]] const std::string& get_name() const { return name_; }

    private:
        std::string name_;

        bool requires_full_hp_ = false;

        std::unique_ptr<IStationManager> manager_;
        std::function<bool()> run_;
    };
}

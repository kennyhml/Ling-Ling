#pragma once
#include <chrono>

namespace llpp::core
{
    class IStationManager
    {
    public:
        IStationManager() = default;
        virtual ~IStationManager() = default;

        virtual bool run() = 0;
        [[nodiscard]] virtual bool is_ready_to_run() = 0;
        [[nodiscard]] virtual std::chrono::minutes get_time_left_until_ready() const = 0;

        IStationManager(const IStationManager&) = delete;
        IStationManager& operator=(const IStationManager&) = delete;
        IStationManager(IStationManager&&) = delete;
        IStationManager& operator=(IStationManager&&) = delete;
    };
}

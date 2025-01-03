#pragma once
#include <chrono>
#include <functional>

namespace lingling
{
    enum class startup_time
    {
        STARTUP_EARLY,
        STARTUP_LATE
    };

    /**
     * @brief Adds a function to be called at startup, this may be used to register callbacks
     * on initialization performing all actions until the project reaches the startup phase.
     */
    void defer_to_startup( startup_time time, const std::function<void()>&);

    /**
     * @brief Helper struct to register a deferred startup callback when static the
     * static initialization of this object occurs. This way modules can register startups
     * without having to be explicitly called to do so by the core application.
     */
    struct run_on_startup final
    {
        explicit run_on_startup(const startup_time when, const std::function<void()>& fn)
        {
            defer_to_startup(when, fn);
        }
    };

    /**
     * @brief Perform the project startup, should only be called once.
     *
     * Runs the callbacks that were deferred to startup, validates the configuration
     * and boots the integrations.
     */
    void startup();

    /**
     * @brief Gets the point of time where the application startup occurred.
     */
    std::chrono::system_clock::time_point get_startup_time();
}

#pragma once
#include <functional>

namespace lingling
{
    /**
     * @brief Adds a function to be called at startup, this may be used to register callbacks
     * on initialization performing all actions until the project reaches the startup phase.
     */
    void defer_to_startup(const std::function<void()>&, bool early_startup = true);

    /**
     * @brief Helper struct to register a deferred startup callback when static the
     * static initialization of this object occurs. This way modules can register startups
     * without having to be explicitly called to do so by the core application.
     */
    struct run_on_startup final
    {
        explicit run_on_startup(const std::function<void()>& fn,
                                const bool early_startup = true)
        {
            defer_to_startup(fn, early_startup);
        }
    };

    /**
     * @brief Perform the project startup, should only be called once.
     *
     * Runs the callbacks that were deferred to startup, validates the configuration
     * and boots the integrations.
     */
    void startup();
}

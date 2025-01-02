#pragma once
#include <cstdint>

namespace lingling
{
    enum class app_state : int32_t
    {
        APP_STATE_ONLINE,  // The application is running but not performing tasks
        APP_STATE_OFFLINE, // The application is not running
        APP_STATE_PAUSED,  // The application is running but paused
        APP_STATE_RUNNING, // The application is running & executing tasks
    };

    /**
    * @brief Sets the state of the application to the provided state. This has no effect
    * on the application itself, i.e if you wish to terminate the application you must do
    * so through the @link terminate_application \endlink function.
    *
    * @param state The new state of the application.
    */
    void set_application_state(app_state state);

    /**
     * @brief Gets the current state of the application, the state must have been previously
     * set by @link set_application_state \endlink or, if it was not yet set, it defaults to
     * the "online" state.
     */
    [[nodiscard]] app_state get_application_state();

    /**
     * @brief Terminates all managed threads running in the current application.
     * The state is set to "online" upon termination.
     *
     * @remark If the application is already terminated, this has no effect.
     */
    void terminate_application();

    /**
     * @brief Starts all managed threads required to run the application.
     *
     * @remark If the application is already running, this has no effect.
     */
    void start_application();
}

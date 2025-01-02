#pragma once
#include <functional>
#include <message.h>

namespace lingling::discord
{
    // Listener function that can gets informed when the dashboard embed is created and
    // may add it's own fields.
    using dashboard_create_listener_t = std::function<void(dpp::embed&)>;

    /**
     * @brief Updates the dashboard with a new embed, the basic discord shows the current
     * runtime of the bot and whether it is idle or active. Concrete modules may attach
     * creation listeners to add their own fields to display information.
     *
     * @remark If no dashboard channel is set, the function will do nothing.
     */
    void update_dashboard();

    /**
     * @brief Formats the given value according to the field formatting convention.
     *
     * @param value The value to format.
     *
     * @return The proper format for the embed field, e.g ">>> **my_value123**".
     */
    std::string fmt_field(const std::string& value);

    /**
     * @brief Adds a listener to be informed when a dashboard is created to update the
     * current dashboard in the channel, the functions receive the embed by reference and
     * may add their own fields / data to it.
     */
    void add_dashboard_creation_listener(dashboard_create_listener_t);




}

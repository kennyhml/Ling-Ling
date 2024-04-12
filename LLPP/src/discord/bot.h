#pragma once
#include <dpp/dpp.h>

namespace llpp::discord
{
    /**
     * @brief Initializes the discord bot with the entered configuration.
     *
     * @return True if successful, false otherwise.
     */
    bool init();

    /**
     * @brief Returns a pointer to the current dpp::cluster bot instance.
     */
    dpp::cluster* get_bot();

    /**
     * @brief Gets the error channel from the config, falls back to info channel.
     */
    dpp::snowflake get_error_channel();
}

#pragma once
#include "common.h"

namespace discord
{
    /**
     * @brief Creates the /config command group designed to to allow the user to change
     * the configuration files. May register multiple subcommands for ease of use.
     *
     * @return The slashcommand and the function to handle the command being triggered.
     */
    [[nodiscard]] slashcommand_and_callback_t create_config_command();
}

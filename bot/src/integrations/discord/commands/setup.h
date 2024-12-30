#pragma once
#include "common.h"

namespace discord
{
    /**
     * @brief Creates the /setup command group designed to handle the setup of a
     * category and channel with the relevant permissions.
     *
     * @return The slashcommand and the function to handle the command being triggered.
     */
    [[nodiscard]] command_register_t create_setup_command(const dpp::snowflake& app_id);
}

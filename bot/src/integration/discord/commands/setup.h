#pragma once
#include "common.h"

namespace lingling::discord
{
    /**
     * @brief Creates the /setup command group designed to handle the setup of a
     * category and channel with the relevant permissions.
     *
     * @return The slashcommand and the function to handle the command being triggered.
     */
    [[nodiscard]] command_register_t create_setup_command(const dpp::snowflake& app_id);

    void add_setup_command_create_listener(command_create_t callback);

    discord::command_callback_t triberole(dpp::slashcommand& cmd);
}

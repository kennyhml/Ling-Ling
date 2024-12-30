#pragma once
#include "common.h"

namespace lingling::discord
{
    /**
     * @brief Creates the /config command group designed to to allow the user to change
     * the configuration files. May register multiple subcommands for ease of use.
     *
     * @remark Individual modules should be given the time to attach their listeners first.
     *
     * @return The slashcommand and the function to handle the command being triggered.
     */
    [[nodiscard]] command_register_t create_config_command(const dpp::snowflake& app_id);

    /**
     * @brief Attaches a listener to the /config slashcommand being built, individual
     * components may attach to this callback to register their own subcommand with
     * their own logic to it.
     *
     * @param callback The function to call be called while building the command.
     */
    void add_config_command_create_listener(command_create_t callback);
}

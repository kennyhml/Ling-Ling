#pragma once
#include "configuration/managedvar.h"
#include "discord/commands/common.h"

namespace lingling
{
    void init_core_config();

    /**
     * @brief Validates the core configuration for the values needed required.
     *
     * @param json The data to validate, if empty it will be populated with defaults.
     *
     * @return Whether changes were made to the data.
     */
    bool validate_core_config(json_t& json);

    /**
     * @brief Gets a reference to the json object of the core configuration data.
     */
    json_t& get_core_config();

    /**
     * @brief Makes sure that a username is set in the config, if a username is provided
     * the function does nothing. If the user name is missing, the user will be prompted
     * to enter one.
     */
    void ensure_user_name_provided();

    /**
     * @brief Adds the /config core subcommand group to allow changing the core
     * configuration through discord.
     *
     * @return The function to handle the /config core commands being invoked.
     */
    discord::command_callback_t add_core_config_command(dpp::slashcommand&);

    // core configuration of the project needed by nearly all components
    inline managed_var<std::string> user_name{"user_name", get_core_config};
    inline managed_var<std::string> asa_directory{"asa_directory", get_core_config};
}

#pragma once
#include <dpp/dpp.h>

#include "configuration/managedvar.h"

namespace lingling::discord
{
    // Function type to handle a slashcommand event
    using command_callback_t = std::function<void(const dpp::slashcommand_t&)>;

    // Slashcommand and the callback to handle the command being triggered
    using command_register_t = std::pair<dpp::slashcommand, command_callback_t>;

    // Callback function to attach a subcommand with a callback to handle it being triggered
    using command_create_t = std::function<command_callback_t(dpp::slashcommand&)>;

    // Alias for dpp confirmation callback lambda input
    using conf_t = const dpp::confirmation_callback_t&;

    /**
     * @brief Struct to register a configurable variable on a discord command.
     */
    struct discord_configurable final
    {
        // The variable name that will be displayed as option to change
        std::string variable_name;

        // The name of the parameter we want for this option, e.g "channel"
        std::string parameter_name;

        // The type of the parameter we want for this option, e.g co_string.
        dpp::command_option_type parameter_type;

        // The description of the parameter
        std::string parameter_description;
    };

    /**
     * @brief Helper to creates config control commands.
     *
     * @param name The name of the command, most likely either "get" or "set".
     * @param description The description of the command.
     * @param configs The configuration options to register to the command.
     * @param params Whether to allow parameters for the options, should be false for get.
     *
     * @return The created command that offers control over the configurables.
     */
    [[nodiscard]] dpp::command_option create_config_ctrl_command(const std::string& name,
        const std::vector<discord_configurable>& configs, const std::string& description,
        bool params = true);

    /**
     * @brief Helper function to try get a variant from a command value.
     */
    template<typename T>
    [[nodiscard]] T tget(const dpp::command_value& value)
    {
        try {
            return std::get<T>(value);
        } catch (const std::bad_variant_access&) {
            return T();
        }
    }

    template<typename T>
    void handle_change(const dpp::slashcommand_t& event, const std::string& name,
                       const std::string& param, managed_var<T>& variable)
    {
        // Check for "get" command.
        if (event.command.get_command_interaction().options[0].options[0].name == "get") {
            event.reply(std::format("**{}:** {}", name, variable.get()));
            return;
        }

        const auto value = discord::tget<T>(event.get_parameter(param));
        if (value.empty()) {
            event.reply(std::format("A value must be provided for `{}`.", param));
            return;
        }

        const auto prev = variable.get();
        variable.set(value);
        if (prev.empty()) {
            event.reply(std::format("**{}** set to `{}`.", name, value));
        }
        event.reply(std::format("**{}** changed from `{}` to `{}`.", name, prev, value));
    }
}

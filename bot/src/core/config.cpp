#include "config.h"
#include "discord/commands/config.h"

#include "configuration/validate.h"
#include "utility/utility.h"

#include <iostream>
#include <asa/core/logging.h>

namespace lingling
{
    namespace
    {
        const json_t CORE_CONFIG_SCHEMA = R"(
        {
            "user_name": "",
            "asa_directory": ""
        })"_json;

        const auto USERNAME_SETUP_MSG =
                "\n======================================================================================\n"
                "\033[1;31mNo user name was found in the configuration data!\033[0m\n"
                "The name is required to properly identifiy the application host of the bot.\n\n"
                "Please enter a user name that:\n"
                "   - Clearly let's your tribemates understand who is running the bot.\n"
                "   - Uniquely identifies the bot machine (if you are running multiple)."
                "\n======================================================================================\n";

        const managed_var_table_t<std::string> value_vars = {
            {"User Name", user_name},
            {"ASA Directory", asa_directory},
        };

        const std::vector<discord::discord_configurable> CONFIG_CHOICES
        {
            {"User Name", "value", dpp::co_string, "The new value."},
            {"ASA Directory", "value", dpp::co_string, "The new value."},
        };

        void handle_config_command(const dpp::slashcommand_t& event)
        {
            const auto var = discord::tget<std::string>(event.get_parameter("variable"));
            if (value_vars.contains(var)) {
                discord::handle_change(event, var, "value", value_vars.at(var));
            } else {
                event.reply(std::format("Variable `{}` is unknown!", var));
            }
        }
    }

    void init_core_config()
    {
        add_config_validation("core", validate_core_config);
        discord::add_config_command_create_listener(add_core_config_command);
    }

    bool validate_core_config(json_t& json)
    {
        return patch_json("core", json, CORE_CONFIG_SCHEMA);
    }

    json_t& get_core_config()
    {
        return get_config_data().at("core");
    }

    void ensure_user_name_provided()
    {
        if (!user_name.get().empty()) {
            asa::get_logger()->info("Known user name: '{}'.", user_name.get());
            return;
        }

        utility::enable_virtual_terminal_processing();
        std::cout << USERNAME_SETUP_MSG;

        std::string name;
        do {
            std::cout << "\033[1;36mUser name\033[0m: ";
            std::getline(std::cin, name);
        } while (name.empty());
        user_name.set(name);
        asa::get_logger()->info("User name set to: '{}'.", user_name.get());
    }

    discord::command_callback_t add_core_config_command(dpp::slashcommand& command)
    {
        dpp::command_option group(dpp::co_sub_command_group, "core",
                                  "Provides control over Ling-Lings core configurations.");

        // Create the set command
        group.add_option(create_config_ctrl_command(
                "set", CONFIG_CHOICES, "Change a variable in the core configuration")
        );

        // Create the get command
        group.add_option(create_config_ctrl_command(
                "get", CONFIG_CHOICES,
                "Get the value of a variable in the core configuration", false)
        );

        command.add_option(group);
        return handle_config_command;
    }
}

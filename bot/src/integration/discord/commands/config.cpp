#include "config.h"


namespace lingling::discord
{
    namespace
    {
        std::vector<command_create_t> command_creation_listeners;
        std::map<std::string, command_callback_t> subcommand_group_callbacks;


        void handle_config_command(const dpp::slashcommand_t& event)
        {
            const auto interaction = event.command.get_command_interaction();
            const auto group = interaction.options[0];

            subcommand_group_callbacks.at(group.name)(event);
        }
    }

    command_register_t create_config_command(const dpp::snowflake& app_id)
    {
        dpp::slashcommand cmd("config", "Read/Modify Ling-Lings configuration.", app_id);

        for (const auto& fn: command_creation_listeners) {
            const auto callback = fn(cmd);
            subcommand_group_callbacks[cmd.options.back().name] = callback;
        }

        return {cmd, handle_config_command};
    }

    void add_config_command_create_listener(command_create_t callback)
    {
        command_creation_listeners.push_back(std::move(callback));
    }
}

#include "commands.h"
#include "../../../config/config.h"
#include "../../../discord/commands/commands.h"

namespace llpp::bots::farm
{
    using namespace config::bots::farm;

    namespace
    {
        void command_callback(const dpp::slashcommand_t& event, void* data);

        discord::ManagedCommand command("metal", "Controls the Metal Managers.", 0,
                                        command_callback, false);

        std::string handle_state_command(const std::string& manager, const bool disable)
        {
            const std::string repr = disable ? "disabled" : "enabled";

            if (manager == "all") {
                for (auto& config : std::views::values(configs)) {
                    config.get_ptr()->disabled = disable;
                    config.save();
                }
                return std::format("All managers {}.", repr);
            }

            if (!configs.contains(manager)) {
                return std::format("No manager named `{}` was found.", manager);
            }

            auto& disabled = configs.at(manager).get_ptr()->disabled;

            if (disable == disabled) {
                return std::format("`{}` is already `{}`.", manager, repr);
            }
            disabled = disable;
            configs.at(manager).save();

            return std::format("`{}` is now `{}`.", manager, repr);
        }

        void command_callback(const dpp::slashcommand_t& event, void* data)
        {
            if (discord::handle_unauthorized_command(event)) { return; }

            auto cmd_data = event.command.get_command_interaction();
            auto option = cmd_data.options[0];

            if (option.name == "disable" || option.name == "enable") {
                const auto manager = option.get_value<std::string>(0);
                return event.reply(
                    handle_state_command(manager, option.name == "disable"));
            }
            return event.reply("Failed to recognize the command");
        }

        dpp::command_option get_disable_or_enable_command(const bool disable)
        {
            const std::string option = disable ? "disable" : "enable";
            const std::string desc = disable
                                         ? "Disable a wood manager by name."
                                         : "Enable a wood manager by name.";

            dpp::command_option choices(dpp::co_string, "manager", "READ.", true);
            choices.add_choice({"all", "all"});
            for (auto& [name, data]: configs) {
                if (data.get_ptr()->type == "METAL") { choices.add_choice({name, name}); }
            }

            return dpp::command_option(dpp::co_sub_command, option, desc)
                    .add_option(choices);
        }
    }

    void register_metal_commands()
    {
        command.options.clear();
        command.add_option(get_disable_or_enable_command(true));
        command.add_option(get_disable_or_enable_command(false));

        discord::register_command(command);
    }
}

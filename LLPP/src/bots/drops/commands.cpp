#include "commands.h"
#include "../../config/config.h"
#include "../../discord/commands/commands.h"

namespace llpp::bots::drops
{
    using namespace config::bots::drops;

    namespace
    {
        void command_callback(const dpp::slashcommand_t& event, void* data);

        discord::ManagedCommand command("crates", "Controls the Crate Managers.", 0,
                                        command_callback, false);

        std::string handle_reroll_command(const std::string& option, const bool enable)
        {
            if (option == "get") {
                return std::format("`Reroll mode` is currently '{}'",
                                   reroll_mode.get() ? "on" : "off");
            }

            const std::string repr = enable ? "on" : "off";
            if (reroll_mode.get() == enable) {
                return std::format("`Reroll mode` is already `{}`", repr);
            }
            reroll_mode.set(enable);
            return std::format("`Reroll mode` is now `{}`", repr);
        }

        std::string handle_state_command(const std::string& manager, const bool disable)
        {
            if (!configs.contains(manager)) {
                return std::format("No manager named `{}` was found.", manager);
            }

            auto& disabled = configs.at(manager).get_ptr()->disabled;
            const std::string repr = disable ? "disabled" : "enabled";

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

            if (option.name == "reroll") {
                const std::string get_or_set = option.options[0].name;
                const bool enable = option.options[0].get_value<bool>(0);
                return event.reply(handle_reroll_command(get_or_set, enable));
            }

            if (option.name == "disable" || option.name == "enable") {
                const auto manager = option.get_value<std::string>(0);
                return event.reply(
                    handle_state_command(manager, option.name == "disable"));
            }
            return event.reply("Failed to recognize the command");
        }

        dpp::command_option get_reroll_command()
        {
            dpp::command_option reroll(dpp::co_sub_command_group, "reroll",
                                       "Toggle the reroll mode on / off.");

            dpp::command_option set(dpp::co_sub_command, "set", "Set reroll mode");
            set.add_option({dpp::co_boolean, "toggle", "Whether to reroll", true});

            reroll.add_option(set)
                  .add_option({dpp::co_sub_command, "get", "Get the reroll state"});

            return reroll;
        }

        dpp::command_option get_disable_or_enable_command(const bool disable)
        {
            const std::string option = disable ? "disable" : "enable";
            const std::string desc = disable
                                         ? "Disable a crate manager by name."
                                         : "Enable a crate manager by name.";

            dpp::command_option choices(dpp::co_string, "manager", "READ.", true);

            for (const auto& name: std::views::keys(configs)) {
                choices.add_choice({name, name});
            }

            return dpp::command_option(dpp::co_sub_command, option, desc)
                    .add_option(choices);
        }
    }

    void register_commands()
    {
        command.options.clear();
        command.add_option(get_reroll_command());
        command.add_option(get_disable_or_enable_command(true));
        command.add_option(get_disable_or_enable_command(false));

        discord::register_command(command);
    }
}

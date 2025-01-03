#include "discord.h"
#include "discord/discord.h"

#include "config.h"

namespace lingling::gacha
{
    namespace
    {
        [[nodiscard]] std::string visualize_preset(const std::string& preset_name)
        {
            const json_t data = get_preset_data(preset_name);
            return std::format("```json\n{}```", data.dump(4));
        }

        void handle_preset_event(const dpp::slashcommand_t& event)
        {
            const auto action = discord::tget<std::string>(event.get_parameter("action"));
            const auto existing_preset = discord::tget<std::string>(
                event.get_parameter("existing-preset"));
            const auto new_preset = discord::tget<std::string>(
                event.get_parameter("new-preset"));

            if (action == "create") {
                add_preset(new_preset);
            } else if (action == "delete") {
                delete_preset(existing_preset);
            } else if (action == "rename") {
                rename_preset(existing_preset, new_preset);
            } else if (action == "show") {
                return event.reply(visualize_preset(existing_preset));
            }
            discord::update_command_list();
            event.reply("**Action completed successfully.**");
        }

        [[nodiscard]] dpp::command_option make_preset_command()
        {
            dpp::command_option cmd(dpp::co_sub_command, "preset",
                                    "Manage your gacha tower preset.");

            cmd.add_option(dpp::command_option(
                    dpp::co_string, "action", "The action you want to perform.", true
                ).add_choice({"New Preset", "create"})
                 .add_choice({"Rename Preset", "rename"})
                 .add_choice({"Delete Preset", "delete"})
                 .add_choice({"Show Preset", "show"})
            );

            dpp::command_option preset(dpp::co_string, "existing-preset",
                                       "The existing preset to perform the chosen action on!",
                                       false);
            for (const auto& name: std::ranges::views::keys(tower_presets)) {
                preset.add_choice({name, name});
            }
            cmd.add_option(preset);
            cmd.add_option({
                dpp::co_string, "new-preset",
                "The new name of the new preset or the name to rename to, must be unique!"
            });
            return cmd;
        }

        void handle_gacha_command(const dpp::slashcommand_t& event)
        {
            const auto interaction = event.command.get_command_interaction();
            const auto subcommand = interaction.options[0].options[0];
            try {
                if (subcommand.name == "preset") {
                    handle_preset_event(event);
                } else {
                    event.reply("Unknown command.");
                }
            } catch (const std::exception& e) {
                event.reply(std::format("**Error: {}**", e.what()));
            }
        }
    }


    discord::command_callback_t add_config_gacha_command_group(dpp::slashcommand& cmd)
    {
        dpp::command_option group(dpp::co_sub_command_group, "gacha",
                                  "Provides control over Ling-Lings Gacha Tower presets.");

        group.add_option(make_preset_command());

        cmd.add_option(group);
        return handle_gacha_command;
    }
}

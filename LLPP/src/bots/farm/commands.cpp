#include "commands.h"
#include "../../discord/bot.h"
#include "farmbot.h"

namespace llpp::bots::farm
{
    namespace
    {
        bool registered = false;

        FarmBot::FarmType type_enum(const std::string& type)
        {
            if (type == "metal") { return FarmBot::METAL; }
            if (type == "wood") { return FarmBot::WOOD; }
            if (type == "obsidian") { return FarmBot::OBSIDIAN; }

            throw std::runtime_error("Cannot get the enum");
        }

        std::string stringify_type(const FarmBot::FarmType type)
        {
            switch (type) {
            case FarmBot::METAL: return "metal";
            case FarmBot::WOOD: return "wood";
            case FarmBot::OBSIDIAN: return "obsidian";
            }
            throw std::runtime_error("Cannot stringify enum");
        }

        void farm_command_callback(const dpp::slashcommand_t& event)
        {
            if (discord::handle_unauthorized_command(event)) { return; }

            auto cmd_data = event.command.get_command_interaction();

            auto subcommand = cmd_data.options[0];
            auto user = event.command.usr;
            FarmBot* bot = FarmBot::instance();

            if (subcommand.name == "start") {
                const auto type = subcommand.get_value<std::string>(0);
                const auto where = subcommand.get_value<std::string>(1);

                if (bot) {
                    return event.reply(std::format(
                        "I am already farming {} with {}. Please end that session first.",
                        stringify_type(bot->get_farm_type()),
                        bot->get_user().get_mention()));
                }

                event.reply(std::format(
                    "Sure {}, I will come to `{}` to farm {} with you soon.",
                    user.get_mention(), where, type));
                new FarmBot(type_enum(type), where, user);
            }

            if (subcommand.name == "end") {
                if (!bot) {
                    return event.reply("There is no active farming session you ape.");
                }
                if (bot->has_started()) { bot->signal_end(); }
                else { bot->destroy(); }
                event.reply("The farming session was ended.");
            }
        }
    }

    void register_commands()
    {
        if (registered) { return; }

        dpp::slashcommand farm("farm", "Start or end a farming session", 0);
        farm.add_option(
            dpp::command_option(dpp::co_sub_command, "start",
                                "Start a new farming session. Ling Ling++ will finish the current task and come to you.")
            .add_option(
                dpp::command_option(dpp::co_string, "type",
                                    "What resource are we farming?",
                                    true).add_choice({"Metal", "metal"}).
                                          add_choice({"Wood", "wood"}).add_choice({
                                              "Obsidian", "obsidian"
                                          })).add_option(dpp::command_option(
                dpp::co_string, "where", "Where should I spawn to mount?", true)));

        farm.add_option(dpp::command_option(dpp::co_sub_command, "end",
                                            "End an active farming session. Ling Ling++ will go back to regular tasks."));

        discord::register_slash_command(farm, farm_command_callback);
        registered = true;
    }
}

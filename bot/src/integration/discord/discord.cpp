#include "discord.h"
#include "config.h"
#include "utility/utility.h"
#include "commands/common.h"
#include "commands/setup.h"

#include <iostream>
#include <asa/core/state.h>
#include <asa/core/logging.h>


namespace lingling::discord
{
    namespace
    {
        // Pointer to the dpp::cluster that will later serve as the bot instance.
        std::shared_ptr<dpp::cluster> bot = nullptr;

        // Slashcommand names mapped to the function to call to handle them, e.g
        // the "setup" command would map to a handle_setup_command function.
        // Subcommands are something the command group has to handle internally.
        std::map<std::string, command_callback_t> command_handlers;

        // Setup message to display when the bot is launched without or with an invalid token
        const auto SETUP_MESSAGE =
                "\n======================================================================================\n"
                "\033[1;34mWelcome to the Ling-Ling++ Discord Bot Setup!\033[0m\n"
                "Authenticating with a Discord Bot is required in order to use the bot.\n\n"
                "This step requires the following:\n"
                "   - The Application Token for the Discord Bot to use.\n"
                "   - A single guild (discord server) with the bot in it.\n\n"
                "Please refer to the discord for detailed instructions on how to obtain this data.\n\n"
                "Should this not be your first launch, your configuration could have been lost.\n"
                "In this case, you may terminate the program now and ask for help in the discord."
                "\n======================================================================================\n";

        /**
         * @brief Gathers the slashcommands from the command components and registers their
         * command handlers to the map.
         *
         * @param app_id The app id of the bot to register the commands to.
         *
         * @return A list of slashcommands the bot should register in the guild.
         */
        [[nodiscard]] std::vector<dpp::slashcommand> gather(const dpp::snowflake& app_id)
        {
            std::vector<dpp::slashcommand> ret;

            // Setup commands
            const auto [cmd, callback] = create_setup_command(app_id);
            ret.push_back(cmd);
            command_handlers.emplace(cmd.name, callback);

            return ret;
        }

        /**
         * @brief Launches the discord bot with the given token.
         *
         * @param token The token to use to launch the bot.
         *
         * @return Whether the discord bot was launched successfully.
         */
        bool launch_discord_bot(const std::string& token)
        {
            bot = std::make_shared<dpp::cluster>(token);

            // Route the logging to our spdlog logger instead.
            bot->on_log([](const dpp::log_t& log) {
                asa::get_logger()->log(
                    static_cast<spdlog::level::level_enum>(log.severity), log.message);
            });

            // Register the slash commands in the guild when ready, global commands can
            // only be registered every few hours so making it guild specific is required.
            bot->on_ready([](const dpp::ready_t&) {
                if (dpp::run_once<struct register_bot_commands>()) {
                    bot->guild_bulk_command_create(gather(bot->me.id), guild_id);
                }
            });

            // Reroute the slashcommand to the components that registered the command.
            bot->on_slashcommand([](const dpp::slashcommand_t& event) {
                command_handlers.at(event.command.get_command_name())(event);
            });

            try {
                bot->start(dpp::st_return);
            } catch (const dpp::invalid_token_exception&) {
                asa::get_logger()->error("Discord Bot startup failed - Token Invalid.");
                return false;
            }
            return true;
        }

        /**
         * @brief Does the first ever startup of the discord bot where a token is not yet
         * known, the token is received via stdin instead.
         *
         * @return True if a first startup was performed, false otherwise.
         */
        bool do_first_startup()
        {
            // Required for colors in the terminal
            utility::enable_virtual_terminal_processing();

            if (!bot_token.get().empty() && !guild_id.get().empty()) {
                return false;
            }
            std::cout << SETUP_MESSAGE;

            // Try to start the discord bot with the provided token to validate it.
            std::string token;
            do {
                std::cout << "\033[1;36mDiscord Bot Token\033[0m: ";
                std::getline(std::cin, token);
            } while (!launch_discord_bot(token));
            bot_token.set(token);
            return true;
        }

        /**
         * @brief Gets the ID of the guild that the bot is used in, if the bot is missing
         * intents or is in less or more than 1 guild, the user is informed of it.
         *
         * The user can then fix the problem and retry by pressing "enter".
         */
        void get_guild()
        {
            bool pending = true;
            bool valid = false;

            asa::get_logger()->info("Trying to obtain guild..");
            bot->current_user_get_guilds([&pending, &valid](conf_t callback) {
                if (callback.is_error()) {
                    asa::get_logger()->error("Could not get guilds, missing intents?");
                    pending = false;
                    return;
                }

                const auto guilds = callback.get<dpp::guild_map>();
                if (guilds.size() != 1) {
                    asa::get_logger()->error(
                        "Bot must be in exactly one guild, not {}!", guilds.size()
                    );
                    pending = false;
                    return;
                }
                // Get the name and ID of the guild and save it for later.
                auto& [id, data] = *guilds.begin();
                asa::get_logger()->info("Guild: '{}' ({}).", data.name, id.str());
                guild_id.set(id);
                valid = true;
                pending = false;
            });

            // Wait for the callback to finish, we cannot proceed until it has.
            while (pending) { asa::checked_sleep(100ms); }
            if (!valid) {
                asa::get_logger()->info("Press enter to try again..");
                std::cin.get();
                return get_guild();
            }
        }
    }

    void initialize_discord_bot()
    {
        // The first startup already starts the bot as part of validating the token,
        // so its not needed to launch the bot again afterwards.
        if (!do_first_startup()) {
            if (!launch_discord_bot(bot_token)) {
                asa::get_logger()->error("Bot could not be started from saved Token.");
                bot_token.set("");
                return initialize_discord_bot();
            }
        }
        get_guild();
    }

    std::shared_ptr<dpp::cluster> get_bot()
    {
        return bot;
    }
}

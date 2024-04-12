#include "bot.h"
#include "../config/config.h"
#include "commands/commands.h"

namespace llpp::discord
{
    namespace
    {
        using namespace config::discord;

        std::unique_ptr<dpp::cluster> bot;

        void validate_configuration()
        {
            using namespace config::discord;
            if (token.get_ptr()->empty()) {
                throw config::BadConfigurationError(
                    "Bot token must be a valid token, not empty.");
            }

            if (guild.get_ptr()->empty()) {
                throw config::BadConfigurationError("Guild ID must not be empty.");
            }
        }

        void ready_callback(const dpp::ready_t& event)
        {
            if (dpp::run_once<struct register_bot_commands>()) {
                auto commands = get_all_commands();
                for (auto& cmd : commands) { cmd.application_id = bot->me.id; }
                bot->guild_bulk_command_create_sync(commands, guild.get());
            }
        }
    }

    bool init()
    {
        validate_configuration();
        bot = std::make_unique<dpp::cluster>(token.get(), dpp::i_all_intents);

        bot->on_slashcommand(slashcommand_callback);
        bot->on_ready(ready_callback);
        bot->on_log(dpp::utility::cout_logger());

        return true;
    }

    dpp::cluster* get_bot() { return bot.get(); }

    dpp::snowflake get_error_channel()
    {
        return channels::error.get_ref().empty()
                   ? channels::info.get()
                   : channels::error.get();
    }
}

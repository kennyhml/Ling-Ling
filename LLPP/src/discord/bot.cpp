#include "bot.h"
#include "../config/config.h"

namespace llpp::discord
{
    namespace
    {
        std::vector<dpp::slashcommand> commands;
        std::map<std::string, event_callback_t> command_callbacks;

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
                for (auto& cmd : commands) { cmd.application_id = bot->me.id; }
                bot->guild_bulk_command_create_sync(commands,
                                                    config::discord::guild.get());
            }
        }

        void slashcommand_callback(const dpp::slashcommand_t& event)
        {
            command_callbacks.at(event.command.get_command_name())(event);
        }
    }

    bool init()
    {
        validate_configuration();
        bot = std::make_unique<dpp::cluster>(config::discord::token.get(), dpp::i_all_intents);

        bot->on_slashcommand(slashcommand_callback);
        bot->on_ready(ready_callback);
        bot->on_log(dpp::utility::cout_logger());

        return true;
    }
}

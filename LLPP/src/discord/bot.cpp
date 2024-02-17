#include "bot.h"
#include "../config/config.h"

namespace llpp::discord
{
    namespace
    {
        using namespace config::discord;

        std::unique_ptr<dpp::cluster> bot;
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
                bot->guild_bulk_command_create_sync(commands, guild.get());
            }
        }

        void slashcommand_callback(const dpp::slashcommand_t& event)
        {
            command_callbacks.at(event.command.get_command_name())(event);
        }

        bool is_role_command_authorized(const dpp::snowflake role)
        {
            const auto allowed = authorization::roles.get_ref();
            return std::ranges::find(allowed, role.str()) != allowed.end();
        }

        bool is_channel_command_authorized(const dpp::snowflake channel)
        {
            return authorization::channels.get_ptr()->empty() || std::ranges::find(
                *authorization::channels.get_ptr(),
                channel.str()) != authorization::channels.get_ptr()->end();
        }

        bool is_user_command_authorized(const dpp::guild_member& user)
        {
            using namespace config::discord::authorization;

            // no roles specified, allow any user to use commands.
            if (users.get_ref().empty() && authorization::roles.get_ref().empty()) {
                return true;
            }
            // check if the user has any of the authorized roles
            if (std::ranges::any_of(user.get_roles(), is_role_command_authorized)) {
                return true;
            }
            // check if the user itself is authorized
            const auto allowed = users.get_ref();
            return std::ranges::find(allowed, user.user_id.str()) != allowed.end();
        }
    }

    bool init()
    {
        validate_configuration();
        bot = std::make_unique<dpp::cluster>(config::discord::token.get(),
                                             dpp::i_all_intents);

        bot->on_slashcommand(slashcommand_callback);
        bot->on_ready(ready_callback);
        bot->on_log(dpp::utility::cout_logger());

        return true;
    }

    dpp::cluster* get_bot() { return bot.get(); }

    bool handle_unauthorized_command(const dpp::slashcommand_t& event)
    {
        if (!is_channel_command_authorized(event.command.channel_id)) {
            event.reply(
                dpp::message("You can't use Ling Ling++ commands in this channel.").
                set_flags(dpp::m_ephemeral));
            return true;
        }
        if (!is_user_command_authorized(event.command.member)) {
            event.reply(
                dpp::message("You are not authorized to use commands.").set_flags(
                    dpp::m_ephemeral));
            return true;
        }
        return false;
    }

    void register_slash_command(const dpp::slashcommand& cmd, const event_callback_t& cb)
    {
        commands.push_back(cmd);
        command_callbacks[cmd.name] = cb;
    }


}

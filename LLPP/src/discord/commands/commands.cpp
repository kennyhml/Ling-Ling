#include "commands.h"

#include "../../config/config.h"

namespace llpp::discord
{
    using namespace config::discord;

    namespace
    {
        std::vector<ManagedCommand> commands_;
        std::vector<ManagedCommand> static_commands_;

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


    void register_command(const ManagedCommand& cmd)
    {
        cmd.is_static() ? static_commands_.push_back(cmd) : commands_.push_back(cmd);
    }

    void clear_commands() { commands_.clear(); }

    void slashcommand_callback(const dpp::slashcommand_t& event)
    {
        for (const auto& command: commands_) {
            if (command.name == event.command.get_command_name()) {
                return command.get_callback()(event, command.get_extra_data());
            }
        }
        for (const auto& command: static_commands_) {
            if (command.name == event.command.get_command_name()) {
                return command.get_callback()(event, command.get_extra_data());
            }
        }
    }

    std::vector<dpp::slashcommand> get_all_commands()
    {
        std::vector<dpp::slashcommand> ret;

        for (auto& cmd: static_commands_) {
            ret.push_back(cmd);
        }

        for (auto& cmd: commands_) {
            ret.push_back(cmd);
        }

        return ret;
    }
}

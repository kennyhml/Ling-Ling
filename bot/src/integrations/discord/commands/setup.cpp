#include "setup.h"

namespace discord
{
    namespace
    {
        // TODO: Get from config
        constexpr auto guild = dpp::snowflake(1117391295808282725);

        // Permissions to be able to view the channel
        constexpr dpp::permission VIEW_CHANNEL = dpp::permission().
                                                 add(dpp::p_view_channel)
                                                 .add(dpp::p_read_message_history);

        // Permissions to be able to write in the channel
        constexpr dpp::permission SEND_MSG = dpp::permission().add(dpp::p_send_messages);

        dpp::channel create_category(const std::string& id, const dpp::snowflake& role)
        {
            auto ch = dpp::channel()
                      .set_type(dpp::CHANNEL_CATEGORY)
                      .set_guild_id(guild);

            ch.set_name(id.empty() ? "Ling Ling++" : std::format("Ling Ling++ [{}]", id));

            // A role was specified for the category, block the @everyone role from seeing
            // the category (same ID as guild) and give the specified role the perms instead.
            if (!role.empty()) {
                ch.set_permission_overwrite(role, dpp::ot_role, VIEW_CHANNEL, SEND_MSG);
                ch.set_permission_overwrite(guild, dpp::ot_role, {}, VIEW_CHANNEL);
            }
            return ch;
        }

        dpp::channel create_tribelog_channel(const dpp::snowflake& category,
                                             const dpp::snowflake& role)
        {
            auto ch = dpp::channel()
                      .set_type(dpp::CHANNEL_TEXT)
                      .set_name("tribelogs")
                      .set_guild_id(guild)
                      .set_parent_id(category);

            // A role was specified for the channel, block the @everyone role from seeing
            // the channel (same ID as guild) and give the specified role the perms instead.
            if (!role.empty()) {
                ch.set_permission_overwrite(role, dpp::ot_role, VIEW_CHANNEL, SEND_MSG);
                ch.set_permission_overwrite(guild, dpp::ot_role, {}, VIEW_CHANNEL);
            }
            return ch;
        }

        void handle_setup_command(dpp::cluster* bot, const dpp::slashcommand_t& e)
        {
            // Extract the command parameters
            const auto id = try_get<std::string>(e.get_parameter("identier"));
            const auto access = try_get<dpp::snowflake>(e.get_parameter("access-role"));
            const auto logs = try_get<dpp::snowflake>(e.get_parameter("tribelog-role"));

            // Category needs to be created first as we need its ID to set as parent
            // for the channels that go into it.
            const auto category = bot->channel_create_sync(create_category(id, access));

            // Use the access role if no tribelog role was specified
            bot->channel_create_sync(create_tribelog_channel(category.id, logs));
        }
    }

    command_register_t create_setup_command(const dpp::snowflake& app_id)
    {
        dpp::slashcommand cmd("setup", "Setup the bot channels in your discord.", app_id);

        cmd.add_option(dpp::command_option(
               dpp::co_string, "identifier",
               "Unique identifier for the category in case there are multiple, for example the server number."
           ))
           .add_option(dpp::command_option(
               dpp::co_role, "access-role",
               "The role members need to have to be able to see the category & channels (everyone if not specified)."
           ))
           .add_option(dpp::command_option(
               dpp::co_role, "command-role",
               "The role members need to have to be able to use bot commands (same as access-role if not specified)."
           ))
           .add_option(dpp::command_option(
               dpp::co_role, "tribelog-role",
               "The role members need to have to be able to see the tribelog (same as access-role if not specified)."
           ));

        return {cmd, handle_setup_command};
    }
}

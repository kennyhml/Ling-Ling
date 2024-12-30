#include "setup.h"

#include "integrations/discord/config.h"

namespace lingling::discord
{
    namespace
    {
        // Permissions to be able to view the channel
        constexpr dpp::permission VIEW_CHANNEL = dpp::permission()
                                                 .add(dpp::p_view_channel)
                                                 .add(dpp::p_read_message_history);

        // Permissions to be able to write in the channel
        constexpr dpp::permission SEND_MSG = dpp::permission().add(dpp::p_send_messages);

        /**
         * @brief Creates the category for the channels are to be setup in given the
         * command parameters relevant to it.
         *
         * @remark Only constructs the category, it does not create it with the bot.
         */
        dpp::channel make_category(const std::string& id, const dpp::snowflake& role)
        {
            auto ch = dpp::channel()
                      .set_type(dpp::CHANNEL_CATEGORY)
                      .set_guild_id(guild_id)
                      .set_name(id);

            // A role was specified for the category, block the @everyone role from seeing
            // the category (same ID as guild) and give the specified role the perms instead.
            if (!role.empty()) {
                ch.set_permission_overwrite(role, dpp::ot_role, VIEW_CHANNEL, SEND_MSG);
                ch.set_permission_overwrite(guild_id, dpp::ot_role, {}, VIEW_CHANNEL);
            }
            return ch;
        }

        /**
         * @brief Creates the dashboard channel, the perms are synced with the category.
         *
         * @remark Only constructs the channel, it does not create it with the bot.
         */
        dpp::channel make_dashboard(const dpp::snowflake& category)
        {
            auto ch = dpp::channel()
                      .set_type(dpp::CHANNEL_TEXT)
                      .set_name("dashboard")
                      .set_guild_id(guild_id)
                      .set_position(1)
                      .set_parent_id(category);
            return ch;
        }

        /**
         * @brief Creates the tribelogs channel given the command parameters relevant to it.
         *
         * @remark Only constructs the channel, it does not create it with the bot.
         */
        dpp::channel make_tribelog(const dpp::snowflake& category,
                                   const dpp::snowflake& role)
        {
            auto ch = dpp::channel()
                      .set_type(dpp::CHANNEL_TEXT)
                      .set_name("tribelogs")
                      .set_guild_id(guild_id)
                      .set_position(2)
                      .set_parent_id(category);

            // A role was specified for the channel, block the @everyone role from seeing
            // the channel (same ID as guild) and give the specified role the perms instead.
            if (!role.empty()) {
                ch.set_permission_overwrite(role, dpp::ot_role, VIEW_CHANNEL, SEND_MSG);
                ch.set_permission_overwrite(guild_id, dpp::ot_role, {}, VIEW_CHANNEL);
            }
            return ch;
        }

        /**
         * @brief Creates the tribelog alert channel given the command parameters relevant to it.
         *
         * @remark Only constructs the channel, it does not create it with the bot.
         */
        dpp::channel make_alerts(const dpp::snowflake& category,
                                 const dpp::snowflake& role)
        {
            auto ch = dpp::channel()
                      .set_type(dpp::CHANNEL_TEXT)
                      .set_name("log-alerts")
                      .set_guild_id(guild_id)
                      .set_position(3)
                      .set_parent_id(category);

            // A role was specified for the channel, block the @everyone role from seeing
            // the channel (same ID as guild) and give the specified role the perms instead.
            if (!role.empty()) {
                ch.set_permission_overwrite(role, dpp::ot_role, VIEW_CHANNEL, SEND_MSG);
                ch.set_permission_overwrite(guild_id, dpp::ot_role, {}, VIEW_CHANNEL);
            }
            return ch;
        }

        /**
         * @brief Performs the actual creation of category and channels.
         */
        void channel_setup_impl(dpp::cluster* bot, const dpp::slashcommand_t& event,
                                const std::string& id)
        {
            const auto access = tget<std::string>(event.get_parameter("access-role"));

            // Create the category and, in the callback confirmation, create the rest of
            // the channels in it. This is because we need the category ID to set as parent
            // in the other channels.
            bot->channel_create(make_category(id, access), [=](conf_t callback) {
                if (callback.is_error()) {
                    return event.edit_original_response({
                        "Setup failed - category could not be created."
                    });
                }
                const auto category = callback.get<dpp::channel>();
                const auto logs = tget<std::string>(event.get_parameter("tribelog-role"));

                // Create the channels without waiting for one another
                bot->channel_create(make_dashboard(category.id));
                bot->channel_create(make_tribelog(category.id, logs));
                bot->channel_create(make_alerts(category.id, logs), [=](conf_t) {
                    event.edit_original_response({"Channel setup completed!"});
                });
            });
        }

        /**
         * @brief Sets up the channels if needed, i.e it first checks whether the category
         * already exists in the guild. The guild must be retrieved from cache so it should
         * be called after a guild_get callback.
         *
         * @remark After all channels are checked and no duplicate was found, control is
         * passed onto the actual channel & category setup implementation.
         */
        void setup_if_needed(dpp::cluster* bot, const dpp::slashcommand_t& event)
        {
            // Set the ID to the default or attach the identifier if supplied.
            auto id = tget<std::string>(event.get_parameter("identifier"));
            id = id.empty() ? "Ling Ling++" : std::format("Ling Ling++ [{}]", id);

            const dpp::guild* guild = find_guild(guild_id);
            if (!guild) {
                event.edit_original_response({"Setup failed - guild not in cache."});
                return;
            }

            const auto pending = std::make_shared<size_t>(guild->channels.size());
            const auto stop = std::make_shared<bool>(false);
            for (const dpp::snowflake& channel: guild->channels) {
                bot->channel_get(channel, [=](conf_t cb) {
                    if (*stop) { return; }

                    if (!cb.is_error() && cb.get<dpp::channel>().name == id) {
                        event.edit_original_response(
                            std::format("Setup failed - `{}` already exists!", id));
                        *stop = true;
                    }

                    // Check if this was the last channel to be checked
                    if (--*pending == 0) { channel_setup_impl(bot, event, id); }
                });
            }
        }

        /**
         * @brief Callback for the /setup command being invoked to complete the category
         * and channel creation.
         *
         * @remark If the category already exists, the command is denied.
         */
        void handle_setup_command(dpp::cluster* bot, const dpp::slashcommand_t& event)
        {
            event.thinking(false, [event, bot](conf_t) {
                // We need to get the guilds channels to check if the category already exists
                bot->guild_get(guild_id, [=](conf_t callback) {
                    if (callback.is_error()) {
                        event.edit_original_response({"Setup failed - guild not found."});
                        return;
                    }
                    setup_if_needed(bot, event);
                });
            });
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

#include "discord.h"
#include "../common/util.h"
#include <asapp/game/window.h>
#include "../config/config.h"

namespace llpp::core::discord
{

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

    bool is_user_command_authorized(const dpp::guild_member& user)
    {
        using namespace config::discord::authorization;

        // no roles specified, allow any user to use commands.
        if (users.get_ptr()->empty() && roles.get_ptr()->empty()) { return true; }

        // check if the user has any of the authorized roles
        if (std::ranges::any_of(user.get_roles(), is_role_command_authorized)) {
            return true;
        }

        // check if the user itself is authorized
        const auto allowed = users.get_ptr();
        return std::ranges::find(*allowed, user.user_id.str()) != allowed->end();
    }

    bool is_channel_command_authorized(const dpp::snowflake channel)
    {
        using namespace config::discord::authorization;

        return channels.get_ptr()->empty() || std::ranges::find(
            *channels.get_ptr(), channel.str()) != channels.get_ptr()->end();
    }

    bool is_role_command_authorized(const dpp::snowflake role)
    {
        const auto allowed = config::discord::authorization::roles.get_ptr();
        return std::ranges::find(*allowed, role.str()) != allowed->end();
    }

    void inform_started()
    {
        auto started_embed = dpp::embed().set_title("Ling Ling++ is starting...").
                                          set_description(std::format(
                                              "Ling Ling++ has been started at <t:{}:t>",
                                              std::chrono::system_clock::to_time_t(
                                                  std::chrono::system_clock::now()))).
                                          set_color(dpp::colors::black).
                                          add_field("Account: ", config::user::name.get(),
                                                    true).add_field(
                                              "Server: ",
                                              config::general::ark::server.get(),
                                              true).set_thumbnail(
                                              "https://static.wikia.nocookie.net/"
                                              "arksurvivalevolved_gamepedia/images/b/b9/"
                                              "Whip_%28Scorched_Earth%29.png/revision/latest/"
                                              "scale-to-width-down/228?cb=20160901213011");

        bot->message_create(dpp::message(config::discord::channels::info.get(),
                                         started_embed));
    }

    void inform_fatal_error(const std::exception& error, const std::string& task)
    {
        dpp::embed embed = dpp::embed().set_title("FATAL! Ling Ling++ has crashed!").
                                        set_description(
                                            "Encountered an unexpected error - manual "
                                            "restart required.").
                                        set_color(dpp::colors::red).
                                        add_field("Error: ", error.what(), true).
                                        add_field("During Task: ", task, true).
                                        set_thumbnail(
                                            "https://static.wikia.nocookie.net/"
                                            "arksurvivalevolved_gamepedia/images/1/1c/"
                                            "Repair_Icon.png/revision/latest?cb=20150731134649")
                                        .set_image("attachment://image.png");

        const auto file_data = util::mat_to_strbuffer(asa::window::screenshot());
        dpp::message message = dpp::message(config::discord::channels::info.get(),
                                            std::format(
                                                "<@&{}>",
                                                config::discord::roles::helper_access.
                                                get())).set_allowed_mentions(
            false, true, false, false, {}, {});
        message.add_file("image.png", file_data, "image/png ").add_embed(embed);

        bot->message_create(message);
    }

    void send_station_disabled(const std::string& station, const std::string& reason)
    {
        dpp::embed embed = dpp::embed();
        embed.set_thumbnail(REPAIR_BROWN);
        embed.set_title(std::format("'{}' has been disabled automatically!", station));
        embed.set_description(
            "Please resolve the issue and re-enable the station.\n"
            "You can do via discord commands or by restarting the bot.");
        embed.add_field("Station: ", station);
        embed.add_field("Reason:", reason);

        embed.set_timestamp(std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now()));
    }

}

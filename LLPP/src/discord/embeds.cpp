#include "embeds.h"
#include "icons.h"
#include "helpers.h"
#include "../config/config.h"
#include <asapp/game/settings.h>
#include <asapp/game/window.h>

#include "bot.h"


namespace llpp::discord
{
    using namespace config::discord;

    dpp::message create_started_message()
    {
        dpp::embed embed;
        embed.set_title("Ling Ling++ has been started!");
        embed.set_color(dpp::colors::black);
        embed.set_thumbnail(WHIP_ICON);
        embed.add_field("Account: ", config::user::name.get(), true);
        embed.add_field("Version: ", "v1.8.1", true);
        embed.add_field("Server: ", asa::settings::last_session_0.get());
        set_now_timestamp(embed);

        return {channels::info.get(), embed};
    }


    dpp::message create_stopped_message()
    {
        dpp::embed embed;
        embed.set_title("Ling Ling++ has been stopped!");
        embed.set_color(dpp::colors::red);
        embed.set_thumbnail(STAMINADRAIN_ICON);
        embed.add_field("Server: ", asa::settings::last_session_0.get());
        set_now_timestamp(embed);

        return {channels::info.get(), embed};
    }

    dpp::message create_paused_message()
    {
        dpp::embed embed;
        embed.set_title("Ling Ling++ has been paused!");
        embed.set_color(dpp::colors::black);
        embed.set_thumbnail(EXHAUSTED_ICON);
        embed.add_field("Server: ", asa::settings::last_session_0.get());
        set_now_timestamp(embed);

        return {channels::info.get(), embed};
    }

    dpp::message create_unpaused_message()
    {
        dpp::embed embed;
        embed.set_title("Ling Ling++ has been unpaused!");
        embed.set_color(dpp::colors::black);
        embed.set_thumbnail(GAININGXP_ICON);
        embed.add_field("Server: ", asa::settings::last_session_0.get());
        set_now_timestamp(embed);

        return {channels::info.get(), embed};
    }

    dpp::message create_fatal_error_message(const std::exception& error,
                                            const std::string& task)
    {
        dpp::embed embed;
        embed.set_title("Ling Ling++ has crashed!");
        embed.set_color(dpp::colors::red);
        embed.set_thumbnail(REPAIR_RED_ICON);
        embed.set_description(
            "Encountered an unexpected error - manual restart required.");
        embed.add_field("Error: ", error.what(), true);
        embed.add_field("Task: ", task, true);
        embed.set_image("attachment://image.png");
        set_now_timestamp(embed);

        dpp::message msg(get_error_channel(), embed);
        msg.set_content(dpp::utility::role_mention(roles::helper_access.get()));
        msg.set_allowed_mentions(false, true, false, false, {}, {});
        msg.add_file("image.png", strbuf(asa::window::screenshot()), "image/png");
        return msg;
    }

    dpp::message create_error_message(const std::string& error, const bool ping)
    {
        dpp::embed embed;
        embed.set_title("Ling Ling++ ran into a problem!");
        embed.set_color(dpp::colors::red);
        embed.set_thumbnail(REPAIR_BROWN_ICON);
        embed.set_description("The error was handled accordingly.");
        embed.add_field("Error: ", error, true);
        embed.set_image("attachment://image.png");
        set_now_timestamp(embed);

        dpp::message msg(get_error_channel(), embed);
        if (ping) {
            msg.set_content(dpp::utility::role_mention(roles::helper_no_access.get()));
            msg.set_allowed_mentions(false, true, false, false, {}, {});
        }
        msg.add_file("image.png", strbuf(asa::window::screenshot()), "image/png");
        return msg;
    }

    dpp::message create_station_disabled_message(const std::string& station,
                                                 const std::string& reason)
    {
        dpp::embed embed;
        embed.set_title(std::format("'{}' has been disabled automatically!", station));
        embed.set_color(dpp::colors::beige);
        embed.set_thumbnail(REPAIR_BROWN_ICON);
        embed.set_description(
            "Please resolve the issue and re-enable the station.\n"
            "You can do so via discord commands or by restarting the bot.");
        embed.add_field("Station: ", station, true);
        embed.add_field("Reason:", reason, true);
        set_now_timestamp(embed);

        dpp::message msg(get_error_channel(), embed);
        msg.set_allowed_mentions(false, true, false, false, {}, {});
        return msg;
    }

    dpp::message create_station_suspended_message(const std::string& station,
                                                  const std::string& reason,
                                                  const std::chrono::minutes duration)
    {
        dpp::embed embed;
        embed.set_title(std::format("'{}' has been suspended!", station));
        embed.set_color(dpp::colors::brown);
        embed.set_thumbnail(TIME_GLASS_ICON);
        embed.set_description(
            "Completion will not be attempted while suspended.\n"
            "You can lift the suspension by re-enabling the station via command.");

        embed.add_field("Station: ", station, true);
        embed.add_field("Reason:", reason, true);
        embed.add_field("Duration: ", std::format("{} seconds", duration.count()), true);
        set_now_timestamp(embed);

        dpp::message msg(get_error_channel(), embed);
        return msg;
    }
}

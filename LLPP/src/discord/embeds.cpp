#include "embeds.h"
#include "icons.h"
#include "helpers.h"
#include "../config/config.h"
#include "../common/util.h"
#include <asapp/game/settings.h>
#include <asapp/game/window.h>


namespace llpp::discord
{
    dpp::embed get_started_embed()
    {
        dpp::embed embed;

        embed.set_title("Ling Ling++ has been started!");
        embed.set_color(dpp::colors::black);
        embed.set_thumbnail(WHIP_ICON);

        embed.add_field("Account: ", config::user::name.get(), true);
        embed.add_field("Version: ", "v1.7.11", true);
        embed.add_field("Server: ", asa::settings::last_session_0.get());

        set_now_timestamp(embed);
        return embed;
    }

    dpp::embed get_fatal_error_embed(const std::exception& error, const std::string& task)
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
        return embed;
    }

    dpp::embed get_station_disabled_embed(const std::string& station,
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
        return embed;
    }

    dpp::embed get_station_suspended_embed(const std::string& station,
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
        return embed;
    }
}

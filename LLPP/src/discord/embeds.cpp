#include "embeds.h"
#include "icons.h"
#include "../config/config.h"
#include "../common/util.h"
#include <asapp/game/settings.h>
#include <asapp/game/window.h>

#include "helpers.h"


namespace llpp::discord
{
    dpp::embed get_started_embed()
    {
        dpp::embed embed;

        embed.set_title("Ling Ling++ has been started!");
        embed.set_color(dpp::colors::black);
        embed.set_thumbnail(WHIP_ICON);

        embed.add_field("Account: ", config::user::name.get(), true);
        embed.add_field("Server: ", asa::settings::last_session_3.get(), true);

        set_now_timestamp(embed);
        return embed;
    }

    dpp::embed get_fatal_error_embed(const std::exception& error, const std::string& task)
    {
        dpp::embed embed;

        embed.set_title("Ling Ling++ has crashed!");
        embed.set_color(dpp::colors::red);
        embed.set_thumbnail(REPAIR_RED_ICON);
        embed.set_description("Encountered an unexpected error - manual restart required.");

        embed.add_field("Error: ", error.what(), true);
        embed.add_field("Task: ", task, true);
        embed.set_image("attachment://image.png");

        set_now_timestamp(embed);
        return embed;
    }

    dpp::embed get_station_disabled_embed(const std::string& station, const std::string& reason)
    {
        dpp::embed embed;

        embed.set_thumbnail(REPAIR_BROWN_ICON);
        embed.set_title(std::format("'{}' has been disabled automatically!", station));
        embed.set_description(
            "Please resolve the issue and re-enable the station.\n"
            "You can do via discord commands or by restarting the bot.");
        embed.add_field("Station: ", station, true);
        embed.add_field("Reason:", reason, true);

        set_now_timestamp(embed);
        return embed;
    }


}

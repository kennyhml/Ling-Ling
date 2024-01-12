#include "embeds.h"
#include "../../common/util.h"
#include "../../core/basestation.h"
#include "../../core/discord.h"
#include "../../core/stationresult.h"
#include <asapp/game/window.h>
#include <format>

#include "../../config/config.h"


namespace llpp::bots::paste
{
    namespace
    {
        const char* PASTE_ICON_URL =
            "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/0/03/Cementing_Paste.png/revision/latest?cb=20180801020251";

        const char* GRINDER_ICON_URL =
            "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/f/fe/Industrial_Grinder.png/revision/latest/scale-to-width-down/228?cb=20160728174054";
    }

    void send_paste_collected(const core::StationResult& data)
    {
        auto next = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now() + data.get_station()->
                                                    get_completion_interval());

        auto embed = dpp::embed();
        embed.set_color(dpp::colors::gray).
              set_title(std::format("Paste Station '{}' has been completed.",
                                    data.get_station()->get_name())).
              set_description(std::format(
                  "The station was completed successfully {} times!",
                  data.get_station()->get_times_completed())).
              set_thumbnail(PASTE_ICON_URL).add_field(
                  "Time taken:", std::format("{} seconds", data.get_time_taken().count()),
                  true).add_field("Paste obtained:",
                                  std::to_string(
                                      data.get_obtained_items().at("Achatina Paste")),
                                  true).add_field("Next completion:",
                                                  std::format("<t:{}:R>", next), true);

        auto msg = dpp::message(config::discord::channels::info.get(), embed);
        core::discord::bot->message_create(msg);
    }

    void send_paste_grinded(const core::StationResult& data, const bool grinder_emptied)
    {
        auto next = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now() + data.get_station()->
                                                    get_completion_interval());

        auto embed = dpp::embed();
        embed.set_color(dpp::colors::white).
              set_title(std::format("Grind Station '{}' has been completed.",
                                    data.get_station()->get_name())).set_description(
                  std::format("The station was completed successfully {} times!",
                              data.get_station()->get_times_completed())).
              set_thumbnail(GRINDER_ICON_URL).add_field(
                  "Time taken:", std::format("{} seconds", data.get_time_taken().count()),
                  true).add_field("Grinder emptied:", grinder_emptied ? "Yes" : "No",
                                  true).add_field("Next completion:",
                                                  std::format("<t:{}:R>", next), true);

        auto msg = dpp::message(config::discord::channels::info.get(), embed);
        core::discord::bot->message_create(msg);
    }

    void send_achatina_not_accessible(const std::string& station_name,
                                      const std::string& achatina_name)
    {
        if (!core::discord::bot) {
            std::cerr << "[!] Skipped posting paste success, no discord bot\n";
            return;
        }

        auto embed = dpp::embed();
        embed.set_color(0xf6c330).
              set_title(std::format("Problem with Achatina at '{}'!", station_name)).
              set_description(std::format(
                  "An achatina may be inaccessible, manual check required.",
                  achatina_name)).set_thumbnail(
                  "https://static.wikia.nocookie.net/" "arksurvivalevolved_gamepedia/"
                  "images/f/f6/Radiation.png/revision/latest?cb=20171216191751").
              add_field("Station:", station_name, true).add_field(
                  "Achatina: ", achatina_name, true).set_image("attachment://image.png");

        auto fileData = util::mat_to_strbuffer(asa::window::screenshot());
        dpp::message message = dpp::message(config::discord::channels::info.get(),
                                            std::format(
                                                "<@&{}>",
                                                config::discord::roles::helper_access.
                                                get())).set_allowed_mentions(
            false, true, false, false, {}, {});
        message.add_file("image.png", fileData, "image/png ").add_embed(embed);
        core::discord::bot->message_create(message);
    }
}

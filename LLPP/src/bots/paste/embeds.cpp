#include "embeds.h"
#include "../../common/util.h"
#include "../../core/basestation.h"
#include "../../core/discord.h"
#include "../../core/stationresult.h"
#include <asapp/game/window.h>
#include <format>


namespace llpp::bots::paste
{
    void send_success_embed(const core::StationResult& data)
    {
        if (!core::discord::bot) {
            std::cerr << "[!] Skipped posting paste success, no discord bot\n";
            return;
        }
        auto nextCompletion = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now() + data.get_station()->
                                                    get_completion_interval());

        auto embed = dpp::embed();
        embed.set_color(dpp::colors::gray).
              set_title(std::format("Paste Station '{}' has been completed.",
                                    data.get_station()->get_name())).
              set_description(std::format(
                  "The station was completed successfully {} times!",
                  data.get_station()->get_times_completed())).set_thumbnail(
                  "https://static.wikia.nocookie.net/"
                  "arksurvivalevolved_gamepedia/images/0/03/"
                  "Cementing_Paste.png/revision/latest?cb=20180801020251").add_field(
                  "Time taken:", std::format("{} seconds", data.get_time_taken().count()),
                  true).add_field("Paste obtained:",
                                  std::to_string(
                                      data.get_obtained_items().at("Achatina Paste")),
                                  true).add_field("Next completion:",
                                                  std::format("<t:{}:R>", nextCompletion),
                                                  true);

        auto msg = dpp::message(core::discord::info_channel_id, embed);
        core::discord::bot->message_create(msg);
    }

    void send_achatina_not_accessible(const std::string& stationName,
                                      const std::string& achatinaName)
    {
        if (!core::discord::bot) {
            std::cerr << "[!] Skipped posting paste success, no discord bot\n";
            return;
        }

        auto embed = dpp::embed();
        embed.set_color(0xf6c330).
              set_title(std::format("Problem with Achatina at '{}'!", stationName)).
              set_description(std::format(
                  "An achatina may be inaccessible, manual check required.",
                  achatinaName)).set_thumbnail(
                  "https://static.wikia.nocookie.net/" "arksurvivalevolved_gamepedia/"
                  "images/f/f6/Radiation.png/revision/latest?cb=20171216191751").
              add_field("Station:", stationName, true).add_field(
                  "Achatina: ", achatinaName, true).set_image("attachment://image.png");

        auto fileData = util::mat_to_strbuffer(asa::window::screenshot());
        dpp::message message = dpp::message(core::discord::info_channel_id,
                                            "<@&1181159721433051136>").
            set_allowed_mentions(false, true, false, false, {}, {});
        message.add_file("image.png", fileData, "image/png ").add_embed(embed);
        core::discord::bot->message_create(message);
    }
}

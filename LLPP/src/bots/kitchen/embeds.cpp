#include "embeds.h"
#include "../../common/util.h"
#include "../../config/config.h"
#include "../../core/basestation.h"
#include "../../discord/bot.h"
#include "../../discord/helpers.h"

namespace llpp::bots::kitchen
{
    namespace
    {
        const std::string BASE_URL =
            "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/";

        std::unordered_map<std::string, std::string> icon_map{
            {"Longrass", BASE_URL + "4/4e/Longrass.png"},
            {"Citronal", BASE_URL + "1/14/Citronal.png"},
            {"Rockarrot", BASE_URL + "c/c3/Rockarrot.png"},
            {"Savoroot", BASE_URL + "3/34/Savoroot.png"}
        };

        const char* SAP_URL =
            "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/7/73/Sap.png/revision/latest?cb=20160706145127";
    }

    void send_crops_collected(const core::StationResult& data,
                              const asa::items::Item* item, const int slots_in_fridge)
    {
        const auto next_completion = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now() + data.station->
                                                    get_completion_interval());

        auto embed = dpp::embed();
        embed.set_color(dpp::colors::orange).
              set_title(std::format("Crop Station '{}' has been completed.",
                                    data.station->get_name())).
              set_description(std::format(
                  "The station was completed successfully {} times!",
                  data.station->get_times_completed())).
              set_thumbnail(icon_map[item->get_name()]).add_field(
                  "Time taken:", std::format("{} seconds", data.time_taken.count()),
                  true).add_field("Fridge:", std::format("{}/80", slots_in_fridge), true).
              add_field("Next completion:", std::format("<t:{}:R>", next_completion),
                        true);

        const auto message = dpp::message(config::discord::channels::info.get(), embed);
        discord::get_bot()->message_create(message);
    }

    void send_sap_collected(const core::StationResult& data, const int slots_in_storage)
    {
        const auto next_completion = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now() + data.station->
                                                    get_completion_interval());

        auto embed = dpp::embed();
        embed.set_color(dpp::colors::yellow).
              set_title(std::format("Sap Station '{}' has been completed.",
                                    data.station->get_name())).
              set_description(std::format(
                  "The station was completed successfully {} times!",
                  data.station->get_times_completed())).set_thumbnail(SAP_URL).
              add_field("Time taken:",
                        std::format("{} seconds", data.time_taken.count()),
                        true).add_field("Storage:",
                                        std::format("{}/45", slots_in_storage),
                                        true).add_field(
                  "Next completion:", std::format("<t:{}:R>", next_completion), true);

        if (slots_in_storage > 30) {
            embed.set_footer(dpp::embed_footer("Please empty the storage box."));
        }

        dpp::message msg(config::discord::channels::info.get(), embed);

        if (slots_in_storage > 30) {
            msg.set_content(
                dpp::utility::role_mention(
                    config::discord::roles::helper_no_access.get()));
            msg.set_allowed_mentions(false, true, true, false, {}, {});
        }
        discord::get_bot()->message_create(msg);
    }

    void send_saptap_not_accessible(const std::string& station_name)
    {
        auto embed = dpp::embed();
        embed.set_color(0xf6c330).
              set_title(std::format("Problem with Sap Tap at '{}'!", station_name)).
              set_description(std::format(
                  "A sap tap may be inaccessible, manual check required.")).set_thumbnail(
                  "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/"
                  "7/73/Sap.png/revision/latest?cb=20160706145127").
              add_field("Station:", station_name, true).set_image("attachment://image.png");

        auto fileData = discord::strbuf(asa::window::screenshot());
        dpp::message message = dpp::message(discord::get_error_channel(),
                                            std::format(
                                                "<@&{}>",
                                                config::discord::roles::helper_no_access.get())).
            set_allowed_mentions(false, true, false, false, {}, {});
        message.add_file("image.png", fileData, "image/png ").add_embed(embed);
        if (config::bots::paste::mute_pings.get()) {
            message.set_content("");
        }
        discord::get_bot()->message_create(message);
    }

    void send_sapstorage_not_accessible(const std::string& station_name)
    {
        auto embed = dpp::embed();
        embed.set_color(0xf6c330).
              set_title(std::format("Problem with Sap Storage at '{}'!", station_name)).
              set_description(std::format(
                  "The sap storage may be inaccessible, manual check required.")).set_thumbnail(
                  "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/"
                  "7/73/Sap.png/revision/latest?cb=20160706145127").
              add_field("Station:", station_name, true).set_image("attachment://image.png");

        auto fileData = discord::strbuf(asa::window::screenshot());
        dpp::message message = dpp::message(discord::get_error_channel(),
                                            std::format(
                                                "<@&{}>",
                                                config::discord::roles::helper_no_access.get())).
            set_allowed_mentions(false, true, false, false, {}, {});
        message.add_file("image.png", fileData, "image/png ").add_embed(embed);
        if (config::bots::paste::mute_pings.get()) {
            message.set_content("");
        }
        discord::get_bot()->message_create(message);
    }
}

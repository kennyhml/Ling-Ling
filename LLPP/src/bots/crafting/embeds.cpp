#include "embeds.h"

#include "../../common/util.h"
#include "../../config/config.h"
#include "../../core/discord.h"
#include "../../core/basestation.h"

namespace llpp::bots::crafting
{
    namespace
    {
        const char* const SPARK_ICON_URL =
            "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/5/56/Sparkpowder.png/revision/latest?cb=20150615114856";
        const char* const GP_ICON_URL =
            "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/a/ae/Gunpowder.png/revision/latest?cb=20150701125817";
        const char* const ARB_ICON_URL =
            "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/6/64/Advanced_Rifle_Bullet.png/revision/latest?cb=20150615121742";

        const char* const WARNING =
            "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/5/53/Mission_Area.png/revision/latest?cb=20200314145130";
    }


    void send_spark_crafted(const core::StationResult& data)
    {
        auto next_completion = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now() + data.get_station()->
                                                    get_completion_interval());

        auto embed = dpp::embed();
        embed.set_color(dpp::colors::orange).
              set_title(std::format("Sparkpowder Station '{}' has been looted!",
                                    data.get_station()->get_name())).set_description(
                  std::format("Crafted spark successfully {} times!",
                              data.get_station()->get_times_completed())).
              set_thumbnail(SPARK_ICON_URL).add_field("Time taken:",
                                                      std::format(
                                                          "{} seconds",
                                                          data.get_time_taken().count()),
                                                      true).add_field(
                  "Next completion:", std::format("<t:{}:R>", next_completion), true);

        const auto message = dpp::message(config::discord::channels::info.get(), embed);
        core::discord::bot->message_create(message);
    }

    void send_gunpowder_crafted(const core::StationResult& data)
    {
        auto next_completion = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now() + data.get_station()->
                                                    get_completion_interval());

        auto embed = dpp::embed();
        embed.set_color(dpp::colors::black).
              set_title(std::format("Gunpowder Station '{}' has been looted!",
                                    data.get_station()->get_name())).set_description(
                  std::format("Crafted gunpowder successfully {} times!",
                              data.get_station()->get_times_completed())).
              set_thumbnail(GP_ICON_URL).add_field("Time taken:",
                                                   std::format(
                                                       "{} seconds",
                                                       data.get_time_taken().count()),
                                                   true).add_field(
                  "Next completion:", std::format("<t:{}:R>", next_completion), true);

        const auto message = dpp::message(config::discord::channels::info.get(), embed);
        core::discord::bot->message_create(message);
    }

    void send_arb_crafted(const core::StationResult& data)
    {
        auto next_completion = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now() + data.get_station()->
                                                    get_completion_interval());

        auto embed = dpp::embed();
        embed.set_color(dpp::colors::black).
              set_title(std::format("ARB Station '{}' has been looted!",
                                    data.get_station()->get_name())).set_description(
                  std::format("Crafted ARB successfully {} times!",
                              data.get_station()->get_times_completed())).
              set_thumbnail(ARB_ICON_URL).add_field("Time taken:",
                                                    std::format(
                                                        "{} seconds",
                                                        data.get_time_taken().count()),
                                                    true).add_field(
                  "Next completion:", std::format("<t:{}:R>", next_completion), true);

        const auto message = dpp::message(config::discord::channels::info.get(), embed);
        core::discord::bot->message_create(message);
    }

    void send_station_capped(const std::string& station_name, const cv::Mat& ss)
    {
        auto embed = dpp::embed();
        embed.set_color(dpp::colors::red).
              set_title(std::format("Dedis at '{}' need to be emptied!", station_name)).
              set_description("The station has been disable automatically.").
              set_thumbnail(WARNING).set_image("attachment://image.png");

        embed.set_footer({
            std::format(
                "Once emptied, you can re-enable the station using /crafting enable {}",
                station_name)
        });
        auto message = dpp::message(config::discord::channels::info.get(), embed);
        const auto fdata = util::mat_to_strbuffer(ss);
        message.add_file("image.png", fdata, "image/png ");

        message.set_content(
            util::get_role_mention(config::discord::roles::helper_no_access.get()));
        message.set_allowed_mentions(false, true, false, false, {}, {});

        core::discord::bot->message_create(message);
    }
}
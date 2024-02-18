#include "embeds.h"

#include "../../common/util.h"
#include "../../config/config.h"
#include "../../discord/bot.h"
#include "../../discord/helpers.h"
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

        const char* GRINDER_ICON_URL =
                "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/f/fe/Industrial_Grinder.png/revision/latest/scale-to-width-down/228?cb=20160728174054";

        const char* FORGE_ICON_URL =
                "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/c/c5/Industrial_Forge.png/revision/latest?cb=20151126023709";
    }

    void send_spark_crafted(const core::StationResult& data, const bool was_crafting)
    {
        auto next_completion = std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::now() + data.get_station()->
                        get_completion_interval());

        auto embed = dpp::embed();
        embed.set_color(dpp::colors::orange)
             .set_title(std::format("Sparkpowder Station '{}' has been looted!",
                                    data.get_station()->get_name()))
             .set_description(
                     std::format("Crafted spark successfully {} times!",
                                 data.get_station()->get_times_completed()))
             .set_thumbnail(SPARK_ICON_URL)
             .add_field("Time taken:",
                        std::format("{} seconds", data.get_time_taken().count()),
                        true)
             .add_field(
                     "Next completion:", std::format("<t:{}:R>", next_completion), true);

        if (was_crafting) {
            embed.set_footer({"Items were still queued, higher interval recommended."});
        }

        const auto message = dpp::message(config::discord::channels::info.get(), embed);
        discord::get_bot()->message_create(message);
    }

    void send_gunpowder_crafted(const core::StationResult& data, const bool was_crafting)
    {
        auto next_completion = std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::now() + data.get_station()->
                        get_completion_interval());

        auto embed = dpp::embed();
        embed.set_color(dpp::colors::black)
             .set_title(std::format("Gunpowder Station '{}' has been looted!",
                                    data.get_station()->get_name()))
             .set_description(
                     std::format("Crafted gunpowder successfully {} times!",
                                 data.get_station()->get_times_completed()))
             .set_thumbnail(GP_ICON_URL)
             .add_field("Time taken:",
                        std::format("{} seconds", data.get_time_taken().count()),
                        true)
             .add_field(
                     "Next completion:", std::format("<t:{}:R>", next_completion), true);

        if (was_crafting) {
            embed.set_footer({"Items were still queued, higher interval recommended."});
        }

        const auto message = dpp::message(config::discord::channels::info.get(), embed);
        discord::get_bot()->message_create(message);
    }

    void send_arb_crafted(const core::StationResult& data, const bool was_crafting)
    {
        auto next_completion = std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::now() + data.get_station()->
                        get_completion_interval());

        const int produced = data.get_obtained_items().at("Advanced Rifle Bullet");

        auto embed = dpp::embed();
        embed.set_color(0xC59A7B)
             .set_title(std::format("ARB Station '{}' has been completed!",
                                    data.get_station()->get_name()))
             .set_description(std::format("Crafted ARB successfully {} times!",
                                          data.get_station()->get_times_completed()))
             .set_thumbnail(ARB_ICON_URL)
             .add_field("Time taken:",
                        std::format("{} seconds", data.get_time_taken().count()), true)
             .add_field("Produced:", "~" + std::to_string(produced), true)
             .add_field("Next completion:", std::format("<t:{}:R>", next_completion),
                        true);

        if (was_crafting) {
            embed.set_footer({"Items were still queued, higher interval recommended."});
        }

        const auto message = dpp::message(config::discord::channels::info.get(), embed);
        discord::get_bot()->message_create(message);
    }

    void send_station_capped(const std::string& station_name, const cv::Mat& ss)
    {
        auto embed = dpp::embed();
        embed.set_color(dpp::colors::red).
                set_title(std::format("Dedis at '{}' need to be emptied!", station_name)).
                     set_description(
                "- The station has been disable automatically.\n"
                "- Re-enable it via command or restart the bot.\n"
                "- The overproduction has been put back.").set_thumbnail(WARNING).
                     set_image("attachment://image.png");

        embed.set_footer({
                                 std::format(
                                         "Empty the dedis and use /crafting enable {}.",
                                         station_name)
                         });
        auto message = dpp::message(config::discord::channels::info.get(), embed);
        const auto fdata = discord::strbuf(ss);
        message.add_file("image.png", fdata, "image/png ");

        message.set_content(
                dpp::utility::role_mention(config::discord::roles::helper_no_access.get()));
        message.set_allowed_mentions(false, true, false, false, {}, {});

        discord::get_bot()->message_create(message);
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
        discord::get_bot()->message_create(msg);
    }

    void send_forge_cycled(const core::StationResult& data, const std::string& item,
                           const int slots_added)
    {
        auto next = std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::now() + data.get_station()->
                        get_completion_interval());

        const int took_slots = data.get_obtained_items().at(item);

        auto embed = dpp::embed();
        embed.set_color(0x454545)
             .set_title(std::format("Forge Station '{}' has been completed.",
                                    data.get_station()->get_name()))
             .set_description(
                     std::format("This forge has been cycled {} times!",
                                 data.get_station()->get_times_completed()))
             .set_thumbnail(FORGE_ICON_URL)
             .add_field("Time taken:",
                        std::format("{} seconds", data.get_time_taken().count()), true)
             .add_field("Item cycled:", item, true)
             .add_field("Next completion:", std::format("<t:{}:R>", next), true)
             .add_field("Processed:", std::to_string(took_slots), true)
             .add_field("Refilled:", std::to_string(slots_added), true)
             .add_field("", "", true);

        auto msg = dpp::message(config::discord::channels::info.get(), embed);
        discord::get_bot()->message_create(msg);

    }


}

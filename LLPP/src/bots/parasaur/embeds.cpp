#include "embeds.h"
#include "../../discord/bot.h"
#include "../../config/config.h"
#include "../../discord/helpers.h"
#include <asapp/game/window.h>

namespace llpp::bots::parasaur
{
    inline const char* const SENSOR_ICON =
        "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/1/16/Tek_Sensor_%28Genesis_Part_1%29.png/revision/latest/scale-to-width-down/228?cb=20200226080818";


    void send_enemy_detected(const std::string& where, const int tag_level)
    {
        dpp::embed embed;

        embed.set_title(std::format("Enemy detected at '{}'!", where));
        embed.set_description(
            "An enemy player or dinosaur was detected!");
        embed.set_color(dpp::colors::cyan);
        embed.set_thumbnail(SENSOR_ICON);

        const auto file_data = discord::strbuf(asa::window::screenshot());
        embed.set_image("attachment://image.png");

        embed.set_timestamp(std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now()));

        dpp::message msg(config::discord::channels::alert.get(), embed);
        msg.add_file("image.png", file_data, "image/png");
        msg.set_allowed_mentions(false, true, true, false, {}, {});

        // Post to the webhook if one is set
        if(!config::discord::webhooks::alert.get_ref().empty()) {
            msg.set_content(dpp::utility::role_mention(config::discord::roles::webhook_alert.get()));
            dpp::webhook wh(config::discord::webhooks::alert.get());
            discord::get_bot()->execute_webhook(wh, msg);
        }

        switch (tag_level) {
        case 0:
        {
            msg.set_content(dpp::utility::role_mention(config::discord::roles::alert.get()));
            break;
        }
        case 1:
            msg.set_content("@here");
            break;
        case 2:
        {
            msg.set_content("@everyone");
            break;
        }
        default:
            break;
        }

        discord::get_bot()->message_create(msg);
    }

    dpp::message get_summary_message(const std::chrono::seconds total_time_taken,
                                     const std::vector<ParasaurStationResult>& stats)
    {
        const int total_ran = std::ranges::count_if(
            stats, [](const auto& result) -> bool { return result.ran; });

        const auto fmt_taken = std::format("{} seconds", total_time_taken.count());
        const auto fmt_checked = std::format("{}/{}", total_ran, stats.size());

        dpp::embed embed;
        embed.set_color(dpp::colors::cyan);
        embed.set_title("Parasaur Stations have been completed!");
        embed.set_description("Here is an overview of the stations:");
        embed.set_thumbnail(PARASAUR_THUMBNAIL);

        embed.add_field("Time taken:", fmt_taken, true);
        embed.add_field("Total checked:", fmt_checked, true);
        embed.add_field("Mode:", config::bots::parasaur::start_criteria.get(), true);

        std::string formatted_data;

        for (const auto& data : stats) {
            const bool detecting = util::get_elapsed<std::chrono::minutes>(
                data.last_detection).count() < 5;

            std::string data_str;

            if (!data.ran) { data_str += ":hourglass: "; }
            else if (detecting) { data_str += ":alarm_clock: "; }
            else { data_str += ":green_circle: "; }

            data_str += ("**" + data.station->get_name() + "**");
            if (data.ran) {
                data_str += std::format(" (<t:{}:R>)",
                                        std::chrono::system_clock::to_time_t(
                                            data.station->get_last_completion()));
            }
            else {
                data_str += std::format(" (<t:{}:R>)",
                                        std::chrono::system_clock::to_time_t(
                                            data.station->get_next_completion()));
            }
            formatted_data += (data_str + "\n");
        }

        embed.add_field("", formatted_data, true);
        return {config::discord::channels::info.get(), embed};
    }
}

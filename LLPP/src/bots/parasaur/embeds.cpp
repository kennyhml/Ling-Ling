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

    dpp::message parasaur_station_message(const std::string& real_name, const std::string& search_name)
    {
        dpp::message result(config::discord::channels::info.get(), "");

        dpp::embed embed;
        embed.set_color(dpp::colors::cyan);
        embed.set_title("Parasaur Station Checked");
        embed.set_thumbnail(PARASAUR_THUMBNAIL);
        embed.set_description(
                "This station was checked to see if a parasaur is detecting an enemy, or not.");
        embed.add_field("Station name:", real_name, false);
        embed.add_field("Bed/Teleport name:", search_name, false);

        embed.set_timestamp(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
        result.embeds.push_back(embed);

        return result;
    }

    dpp::message get_summary_message(const std::chrono::seconds total_time_taken,
                                     const std::vector<ParasaurStationResult>& stats)
    {
        dpp::message result(config::discord::channels::info.get(), "");

        const int total_ran = std::ranges::count_if(
                stats, [](const auto& result) -> bool { return result.ran; });

        const auto fmt_taken = std::format("{} seconds", total_time_taken.count());
        const auto fmt_checked = std::format("{}/{}", total_ran, stats.size());

        dpp::embed embed;
        embed.set_color(dpp::colors::cyan);
        embed.set_title("Parasaur Stations have been checked!");
        embed.set_thumbnail(PARASAUR_THUMBNAIL);

        embed.add_field("Time taken:", fmt_taken, true);
        embed.add_field("Total checked:", fmt_checked, true);
        embed.add_field("Mode:", config::bots::parasaur::start_criteria.get(), true);

        dpp::embed alternate_header; // Used when parasaur embed spans multiple messages
        alternate_header.set_color(dpp::colors::cyan);
        alternate_header.set_thumbnail(PARASAUR_THUMBNAIL);

        std::string formatted_data;

        for (const auto& data : stats) {
            const bool detecting = util::get_elapsed<std::chrono::minutes>(
                    data.last_detection).count() < 5;

            std::string data_str;

            if (!data.ran) { data_str = ":hourglass: "; }
            else if (detecting) { data_str = ":alarm_clock: "; }
            else { data_str = ":green_circle: "; }

            data_str += ("**" + data.real_name_ + "**");

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
            embed.add_field("", data_str, true);

            // Every 24 fields make a new embed
            // This is useful when there are a LOT of parasaur stations and it won't fit in 1 embed
            if(embed.fields.size() > 15) {
                // Add this embed to the queue
                result.embeds.push_back(embed);
                // Add the new header for the next embed
                embed = alternate_header;
            }
        }

        // Add timestamp to the footer of the embed
        embed.set_timestamp(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
        // Push whatever is left, ready to be posted to Discord
        result.embeds.push_back(embed);

        return result;
    }
}

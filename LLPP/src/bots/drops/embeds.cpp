#include "embeds.h"
#include "../../common/util.h"
#include "../../config/config.h"
#include "../../core/basestation.h"
#include "../../discord/bot.h"
#include <format>
#include <opencv2/highgui.hpp>

#include "../../discord/helpers.h"

namespace llpp::bots::drops
{
    namespace
    {
        auto const WHITE_CRATE_THUMBNAIL =
            "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/8/8c/White_Crate.png/revision/latest?cb=20190116151008";

        auto const BLUE_CRATE_THUMBNAIL =
            "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/9/9c/Blue_Crate.png/revision/latest/scale-to-width-down/109?cb=20190116151056";

        auto const YELLOW_CRATE_THUMBNAIL =
            "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/6/63/Yellow_Crate.png/revision/latest?cb=20190116151124";

        auto const RED_CRATE_THUMBNAIL =
            "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/c/c5/Red_Crate.png/revision/latest?cb=20190116151200";


        void get_color_fields(const int quality, uint32_t& color_out,
                              std::string& thumbnail_out, std::string& quality_out)
        {
            using Quality = asa::structures::CaveLootCrate::Quality;

            switch (quality) {
            case Quality::RED:
            {
                color_out = dpp::colors::red_blood;
                thumbnail_out = RED_CRATE_THUMBNAIL;
                quality_out = "RED";
                break;
            }
            case Quality::YELLOW:
            {
                color_out = dpp::colors::yellow;
                thumbnail_out = YELLOW_CRATE_THUMBNAIL;
                quality_out = "YELLOW";
                break;
            }
            case Quality::BLUE:
            {
                color_out = dpp::colors::blue;
                thumbnail_out = BLUE_CRATE_THUMBNAIL;
                quality_out = "BLUE";
                break;
            }
            default: break;
            }
        }

        dpp::snowflake get_loot_channel()
        {
            if (config::bots::drops::loot_channel.get_ref().empty()) {
                return config::discord::channels::info.get();
            }
            return config::bots::drops::loot_channel.get();
        }
    }


    dpp::message get_looted_message(const core::StationResult& data,
                                    const cv::Mat& loot_image,
                                    const asa::structures::CaveLootCrate::Quality
                                    drop_quality, int total_times_looted)
    {
        // default values for drops we couldnt figure out
        auto color = dpp::colors::white;
        std::string thumbnail = WHITE_CRATE_THUMBNAIL;
        std::string determined_quality = "Undetermined";
        get_color_fields(drop_quality, color, thumbnail, determined_quality);


        const auto loot_ratio = std::format("The crate was looted {}/{} times!",
                                            total_times_looted,
                                            data.station->get_times_completed());
        const auto secs_taken = std::format("{} seconds", data.time_taken.count());
        auto next = std::chrono::system_clock::to_time_t(
            data.station->get_next_completion());

        dpp::embed embed;
        embed.set_title(std::format("Looted '{}'!", data.station->get_name()));
        embed.set_color(color);
        embed.set_thumbnail(thumbnail);
        embed.set_description(loot_ratio);
        embed.add_field("Time taken:", secs_taken, true);
        embed.add_field("Crate Quality:", determined_quality, true);
        embed.add_field("Next completion:", std::format("<t:{}:R>", next), true);

        embed.set_image("attachment://image.png");
        discord::set_now_timestamp(embed);

        auto message = dpp::message(get_loot_channel(), embed);

        const auto filedata = discord::strbuf(loot_image);
        message.add_file("image.png", filedata, "image/png");

        return message;
    }

    dpp::message get_reroll_message(const core::StationResult& data,
                                    const cv::Mat& loot_image,
                                    const asa::structures::CaveLootCrate::Quality
                                    drop_quality,
                                    const std::chrono::system_clock::time_point expires,
                                    const std::vector<LootResult>& looted)
    {
        const auto time_left = std::chrono::system_clock::to_time_t(expires);
        // The reroll request message is essentially the same as the looted message
        // with some minor modifications, so to avoid boilerplate just modify a
        // looted message instead.
        dpp::message message = get_looted_message(data, loot_image, drop_quality, 0);
        dpp::embed& embed = message.embeds[0];

        embed.set_title(std::format("Crate '{}' is up!", data.station->get_name()));
        embed.set_description("If the timer expires, the drop will be looted.");

        embed.fields[0] = {"Expires:", std::format("<t:{}:R>", time_left), true};

        if (!looted.empty()) {
            std::string fmt_string = ">>> **__Automatically looted:__**\n";

            for (int i = 0; i < looted.size(); i++) {
                const std::string emote = looted[i].looted ? ":shopping_cart:" : ":x:";
                std::string name = looted[i].name;
                if (const auto pos = name.find("Blueprint"); pos != std::string::npos) {
                    name.replace(pos, 9, "Bp");
                }
                fmt_string += std::format("{}: {}\n", name, emote);
            }
            embed.add_field("", fmt_string);
        }

        if (!config::bots::drops::reroll_role.get().empty()) {
            message.content = dpp::utility::role_mention(
                config::bots::drops::reroll_role.get());
        }

        message.set_allowed_mentions(false, true, false, false, {}, {});
        return message;
    }


    dpp::message get_summary_message(const std::string& manager_name,
                                     const std::chrono::seconds time_taken,
                                     const std::vector<CrateManager::CrateGroupStatistics>
                                     & stats,
                                     const std::map<std::string, float>&
                                     vault_fill_levels,
                                     const std::chrono::system_clock::time_point
                                     next_completion)
    {
        const std::string secs_taken = std::format("{} seconds", time_taken.count());
        const auto next = std::chrono::system_clock::to_time_t(next_completion);

        dpp::embed embed;
        embed.set_title(std::format("Crates '{}' completed!", manager_name));
        embed.set_color(dpp::colors::yellow);
        embed.set_description("Summary of the associated crates:");
        embed.set_thumbnail(YELLOW_CRATE_THUMBNAIL);

        embed.add_field("Time taken:", secs_taken, true);
        embed.add_field("Next completion:", std::format("<t:{}:R>", next), true);

        std::string vault_data;
        bool any_too_full = false;
        const int threshold = config::bots::drops::vault_alert_threshold.get();

        for (const auto& [vault_name, level] : vault_fill_levels) {
            vault_data += std::format("{}: {}%\n", vault_name,
                                      static_cast<int>(level * 100));
            any_too_full |= (static_cast<int>(level * 100) > threshold);
        }
        embed.add_field("Vaults filled:", vault_data, true);

        // Add the group statistics
        for (int i = 0; i < static_cast<int>(stats.size()); i++) {
            auto respawn = std::chrono::duration_cast<std::chrono::minutes>(
                stats[i].get_avg());
            embed.add_field("", std::format(
                                ">>> **__Group {}:__** \nRespawn: {} "
                                "\nTimes looted: {}", i + 1, respawn,
                                stats[i].get_times_looted()), true);
        }

        dpp::message message(config::discord::channels::info.get(), embed);
        if (any_too_full) {
            message.set_content(dpp::utility::role_mention(
                config::discord::roles::helper_access.get()));
            message.set_allowed_mentions(false, true, false, false, {}, {});
            embed.set_footer({"The vault slot threshold was exceeded, please empty."});
        }
        else { discord::set_now_timestamp(embed); }

        return message;
    }
}

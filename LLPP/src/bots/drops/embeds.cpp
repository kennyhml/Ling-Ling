#include "embeds.h"
#include "../../common/util.h"
#include "../../config/config.h"
#include "../../core/basestation.h"
#include "../../discord/bot.h"
#include <format>
#include <opencv2/highgui.hpp>
#include "checkmark_green.h"
#include "../../discord/helpers.h"
#include <asapp/util/util.h>

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

        void paste_dura(cv::Mat& dst, const int slot, const cv::Mat& dura)
        {
            constexpr int x_offset = 14;
            constexpr int y_offset = 80;
            constexpr int per = 94;

            cv::Mat dura_mask = asa::window::get_mask(dura, {158, 224, 238}, 50);

            const int vertical_middle = dura_mask.rows / 2;
            int zero_count = 0;
            int width = 0;
            for (int col = 0; col < dura_mask.cols; col++) {
                if (zero_count > 15) { break; }
                if (dura_mask.at<uchar>(vertical_middle, col) == 0) { zero_count++; }
                else {
                    width = col;
                    zero_count = 0;
                }
            }
            width -= 5;
            cv::Mat red(cv::Size(width / 2, dura.rows), CV_8UC4);

            for (int y = 0; y < red.rows; ++y) {
                for (int x = 0; x < red.cols; ++x) {
                    if (x > width) { break; }
                    if (dura_mask.at<uchar>(y, x) == 0) {
                        red.at<cv::Vec4b>(y, x) = {0, 0, 0, 255};
                    }
                    else { red.at<cv::Vec4b>(y, x) = {0, 0, 255, 255}; }
                }
            }
            const cv::Rect where(x_offset + (per * slot), y_offset + (per * (slot / 5)),
                                 red.cols, red.rows);
            red.copyTo(dst(where), dura_mask({0, 0, red.cols, red.rows}));
        }

        void paste_checkmark(cv::Mat& dst, const int slot)
        {
            constexpr int x_offset = 77;
            constexpr int y_offset = 80;
            constexpr int per = 94;

            static cv::Mat checkmark = util::bytes_to_mat(checkmark_data, 20, 20, 4);

            const auto mask = ::util::mask_alpha_channel(checkmark);

            const cv::Rect where(x_offset + (per * slot), y_offset + (per * (slot / 5)),
                                 checkmark.cols, checkmark.rows);
            checkmark.copyTo(dst(where), mask);
        }
    }

    dpp::message get_looted_message(const core::StationResult& data,
                                    const cv::Mat& loot_image,
                                    const asa::structures::CaveLootCrate::Quality
                                    drop_quality, int total_times_looted,
                                    const std::vector<LootResult>& contents,
                                    const bool reroll)
    {
        // default values for drops we couldnt figure out
        auto color = dpp::colors::white;
        std::string thumbnail = WHITE_CRATE_THUMBNAIL;
        std::string determined_quality = "Undetermined";
        get_color_fields(drop_quality, color, thumbnail, determined_quality);

        cv::Mat edited_image;
        cv::cvtColor(loot_image, edited_image, cv::COLOR_RGB2RGBA);
        for (int i = 0; i < contents.size(); i++) {
            if (contents[i].tooltip) {
                auto dura = cv::Mat(contents[i].tooltip->get_image(),
                                    contents[i].tooltip->get_durability_area()->to_cv());
                paste_dura(edited_image, i, dura);
            }
            if (contents[i].looted && reroll) {
                paste_checkmark(edited_image, i);
            }
        }

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

        const auto filedata = discord::strbuf(edited_image);
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
        auto msg = get_looted_message(data, loot_image, drop_quality, 0, looted, true);
        dpp::embed& embed = msg.embeds[0];

        embed.set_title(std::format("Crate '{}' is up!", data.station->get_name()));
        embed.set_description("If the timer expires, the drop will be looted.");

        embed.fields[0] = {"Expires:", std::format("<t:{}:R>", time_left), true};

        if (!config::bots::drops::reroll_role.get().empty()) {
            msg.content = dpp::utility::role_mention(
                config::bots::drops::reroll_role.get());
        }

        msg.set_allowed_mentions(false, true, false, false, {}, {});
        return msg;
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

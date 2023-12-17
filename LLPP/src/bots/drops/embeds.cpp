#include "embeds.h"
#include "../../common/util.h"
#include "../../core/basestation.h"
#include "../../core/discord.h"
#include <format>


namespace llpp::bots::drops
{
    namespace
    {
        void get_color_fields(int quality, uint32_t& color_out,
                              std::string& thumbnail_out, std::string& quality_out)
        {
            using Quality = asa::structures::CaveLootCrate::Quality;

            switch (quality)
            {
            case Quality::RED:
                color_out = dpp::colors::red_blood;
                thumbnail_out = RED_CRATE_THUMBNAIL;
                quality_out = "RED";
                break;
            case Quality::YELLOW:
                color_out = dpp::colors::yellow;
                thumbnail_out = YELLOW_CRATE_THUMBNAIL;
                quality_out = "YELLOW";
                break;
            case Quality::BLUE:
                color_out = dpp::colors::blue;
                thumbnail_out = BLUE_CRATE_THUMBNAIL;
                quality_out = "BLUE";
                break;
            default:
                break;
            }
        }
    }

    void send_success_embed(const core::StationResult& data, cv::Mat loot,
                            asa::structures::CaveLootCrate::Quality quality,
                            int times_looted)
    {
        auto color = dpp::colors::white;
        std::string thumbnail = WHITE_CRATE_THUMBNAIL;
        std::string determined_quality = "Undetermined";

        get_color_fields(quality, color, thumbnail, determined_quality);
        auto next_completion = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now() +
            data.get_station()->get_completion_interval());

        auto embed = dpp::embed();
        embed.set_color(color)
             .set_title(std::format(
                 "Crate '{}' has been looted!", data.get_station()->get_name()))
             .set_description(
                 std::format("This crate has been looted {}/{} times!",
                             times_looted, data.get_station()->get_times_completed()))
             .set_thumbnail(thumbnail)
             .add_field("Time taken:",
                        std::format("{} seconds", data.get_time_taken().count()), true)
             .add_field("Crate Quality:", determined_quality, true)
             .add_field("Next completion:",
                        std::format("<t:{}:R>", next_completion), true)
             .set_image("attachment://image.png");

        auto fileData = util::mat_to_strbuffer(loot);
        auto message = dpp::message(1178962441091162173, embed);
        message.add_file("image.png", fileData, "image/png");

        core::discord::bot->message_create(message);
    }

    void request_reroll(const core::StationResult& data, cv::Mat loot,
                        asa::structures::CaveLootCrate::Quality quality,
                        std::chrono::system_clock::time_point expires)
    {
        using Quality = asa::structures::CaveLootCrate::Quality;

        auto color = dpp::colors::white;
        std::string thumbnail = WHITE_CRATE_THUMBNAIL;
        std::string determined_quality = "Undetermined";
        get_color_fields(quality, color, thumbnail, determined_quality);

        auto next_completion = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now() +
            data.get_station()->get_completion_interval());

        auto time_left = std::chrono::system_clock::to_time_t(expires);

        auto embed = dpp::embed();
        embed.set_color(color)
             .set_title(std::format(
                 "Requesting reroll at '{}'!", data.get_station()->get_name()))
             .set_description("If the timer expires, the drop will be looted.")
             .set_thumbnail(thumbnail)
             .add_field("Expires:", std::format("<t:{}:R>", time_left), true)
             .add_field("Crate Quality:", determined_quality, true)
             .add_field("Next completion:",
                        std::format("<t:{}:R>", next_completion), true)
             .set_image("attachment://image.png");

        auto fileData = util::mat_to_strbuffer(loot);
        auto message = dpp::message(
            1178962441091162173, "<@&1184887046184108052>");
        message.set_allowed_mentions(false, true, false, false, {}, {});
        message.add_file("image.png", fileData, "image/png ").add_embed(embed);

        core::discord::bot->message_create(message);
    }

    void send_summary_embed(const std::string& name,
                            std::chrono::seconds time_taken,
                            std::vector<CrateManager::CrateGroupStatistics> stats,
                            int vault_slots,
                            std::chrono::system_clock::time_point next_completion)
    {
        auto embed = dpp::embed();
        embed.set_color(dpp::colors::yellow)
             .set_title(
                 std::format("Crate Manager '{}' has been completed!", name))
             .set_description("Here is a summary of this crate managers data:")
             .set_thumbnail(YELLOW_CRATE_THUMBNAIL)
             .add_field("Time taken:",
                        std::format("{} seconds", time_taken.count()), true)
             .add_field("Vault filled:", std::format("{}%", vault_slots), true)
             .add_field("Next completion:",
                        std::format("<t:{}:R>",
                                    std::chrono::system_clock::to_time_t(
                                        next_completion)),
                        true)
             .set_image("attachment://image.png");

        for (int i = 0; i < stats.size(); i++)
        {
            auto respawnTime = std::chrono::duration_cast<std::chrono::minutes>(
                stats[i].get_avg_respawn());

            embed.add_field("",
                            std::format(">>> **__Group {}:__** \nRespawn: {} "
                                        "\nTimes looted: {}",
                                        i + 1, respawnTime, stats[i].get_times_looted()),
                            true);
        }

        auto message = dpp::message(1178195307482325072, embed);
        core::discord::bot->message_create(message);
    }
}

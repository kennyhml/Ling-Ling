#include "embeds.h"
#include "../../common/util.h"
#include "../../config/config.h"
#include "../../core/basestation.h"
#include "../../discord/bot.h"
#include <format>

#include "../../discord/helpers.h"

namespace llpp::bots::drops
{
    namespace
    {
        void get_color_fields(int quality, uint32_t& color_out,
                              std::string& thumbnail_out, std::string& quality_out)
        {
            using Quality = asa::structures::CaveLootCrate::Quality;

            switch (quality) {
            case Quality::RED: color_out = dpp::colors::red_blood;
                thumbnail_out = RED_CRATE_THUMBNAIL;
                quality_out = "RED";
                break;
            case Quality::YELLOW: color_out = dpp::colors::yellow;
                thumbnail_out = YELLOW_CRATE_THUMBNAIL;
                quality_out = "YELLOW";
                break;
            case Quality::BLUE: color_out = dpp::colors::blue;
                thumbnail_out = BLUE_CRATE_THUMBNAIL;
                quality_out = "BLUE";
                break;
            default: break;
            }
        }
    }

    void send_success_embed(const core::StationResult& data, cv::Mat loot,
                            asa::structures::CaveLootCrate::Quality quality,
                            int times_looted, const bool got_rerolled)
    {
        auto color = dpp::colors::white;
        std::string thumbnail = WHITE_CRATE_THUMBNAIL;
        std::string determined_quality = "Undetermined";
        using namespace config::bots::drops;
        get_color_fields(quality, color, thumbnail, determined_quality);
        auto next_completion = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now() + data.station->
                                                    get_completion_interval());

        auto embed = dpp::embed();
        embed.set_color(color).
              set_title(std::format("Crate '{}' has been looted!",
                                    data.station->get_name())).
              set_description(std::format("This crate has been looted {}/{} times!",
                                          times_looted,
                                          data.station->get_times_completed())).
              set_thumbnail(thumbnail).add_field("Time taken:",
                                                 std::format(
                                                     "{} seconds",
                                                     data.time_taken.count()),
                                                 true).add_field(
                  "Crate Quality:", determined_quality, true).add_field(
                  "Next completion:", std::format("<t:{}:R>", next_completion),
                  true).set_image("attachment://image.png");

        if (got_rerolled) { embed.set_footer({"The drop was rerolled and left up."}); }

        const auto fdata = discord::strbuf(loot);
        const auto channel = loot_channel.get_ptr()->empty()
                                 ? config::discord::channels::info.get()
                                 : loot_channel.get();
        auto message = dpp::message(channel, embed);
        message.add_file("image.png", fdata, "image/png");

        discord::get_bot()->message_create(message);
    }

    void request_reroll(const core::StationResult& data, cv::Mat loot,
                        asa::structures::CaveLootCrate::Quality quality,
                        std::chrono::system_clock::time_point expires,
                        std::map<std::string, bool> cherry_picked)
    {
        using Quality = asa::structures::CaveLootCrate::Quality;

        auto color = dpp::colors::white;
        std::string thumbnail = WHITE_CRATE_THUMBNAIL;
        std::string determined_quality = "Undetermined";
        get_color_fields(quality, color, thumbnail, determined_quality);
        using namespace config::bots::drops;
        auto next_completion = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now() + data.station->
                                                    get_completion_interval());

        auto time_left = std::chrono::system_clock::to_time_t(expires);

        auto embed = dpp::embed();
        embed.set_color(color).
              set_title(std::format("Requesting reroll at '{}'!",
                                    data.station->get_name())).
              set_description("If the timer expires, the drop will be looted.").
              set_thumbnail(thumbnail).
              add_field("Expires:", std::format("<t:{}:R>", time_left), true).
              add_field("Crate Quality:", determined_quality, true).add_field(
                  "Next completion:", std::format("<t:{}:R>", next_completion),
                  true).set_image("attachment://image.png");

        if (!cherry_picked.empty()) {
            std::string fmt_string = ">>> **__Automatically looted:__**\n";

            for (auto& [key, looted] : cherry_picked) {
                std::string item = key;
                std::string emote = looted ? ":shopping_cart:" : ":x:";

                const auto pos = item.find("Blueprint");
                if (pos != std::string::npos) { item.replace(pos, 9, "Bp"); }
                fmt_string += std::format("{}: {}\n", item, emote);
            }
            embed.add_field("", fmt_string);
        }

        auto fdata = discord::strbuf(loot);
        const auto channel = loot_channel.get_ptr()->empty()
                                 ? config::discord::channels::info.get()
                                 : loot_channel.get();
        auto message = dpp::message(channel, embed);

        if (!reroll_role.get().empty()) {
            message.content = dpp::utility::role_mention(reroll_role.get());
        }

        message.set_allowed_mentions(false, true, false, false, {}, {});
        message.add_file("image.png", fdata, "image/png ");

       discord::get_bot()->message_create(message);
    }

    void send_summary_embed(const std::string& name,
                            const std::chrono::seconds time_taken,
                            const std::vector<CrateManager::CrateGroupStatistics>& stats,
                            const std::map<std::string, float>& vault_fill_levels,
                            const std::chrono::system_clock::time_point next_completion)
    {
        const auto fmt_taken = std::format("{} seconds", time_taken.count());
        const auto fmt_stamp = std::format("<t:{}:R>",
                                           std::chrono::system_clock::to_time_t(
                                               next_completion));

        auto embed = dpp::embed().set_color(dpp::colors::yellow).
                                  set_title(std::format(
                                      "Crate Manager '{}' has been completed!", name))
                                  .set_description(
                                      "Here is a summary of this crate managers data:").
                                  set_thumbnail(YELLOW_CRATE_THUMBNAIL).add_field(
                                      "Time taken:", fmt_taken, true).add_field(
                                      "Next completion:", fmt_stamp, true);

        // add the vault fill levels
        std::string vault_data;
        bool any_too_full = false;
        const int threshold = config::bots::drops::vault_alert_threshold.get();
        for (auto [vault_name, level] : vault_fill_levels) {
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

        dpp::message msg(config::discord::channels::info.get(), embed);
        if (any_too_full) {
            msg.set_content(dpp::utility::role_mention(
                config::discord::roles::helper_access.get()));
            msg.set_allowed_mentions(false, true, false, false, {}, {});
        }

        discord::get_bot()->message_create(msg);
    }
}

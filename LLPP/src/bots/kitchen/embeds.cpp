#include "embeds.h"
#include "../../common/util.h"
#include "../../config/config.h"
#include "../../core/basestation.h"
#include "../../core/discord.h"

namespace llpp::bots::kitchen
{
    namespace
    {
        const std::string base_url =
            "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/";

        std::unordered_map<std::string, std::string> icon_map{
            {"Longrass", base_url + "4/4e/Longrass.png"},
            {"Citronal", base_url + "1/14/Citronal.png"},
            {"Rockarrot", base_url + "c/c3/Rockarrot.png"},
            {"Savoroot", base_url + "3/34/Savoroot.png"}
        };
    }

    void send_success_embed(const core::StationResult& data, const asa::items::Item* item,
                            const int slots_in_fridge)
    {
        const auto next_completion = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now() +
            data.get_station()->get_completion_interval());

        auto embed = dpp::embed();
        embed.set_color(dpp::colors::orange)
             .set_title(std::format("Crop Station '{}' has been completed.",
                                    data.get_station()->get_name()))
             .set_description(
                 std::format("The station was completed successfully {} times!",
                             data.get_station()->get_times_completed()))
             .set_thumbnail(icon_map[item->get_name()])
             .add_field("Time taken:",
                        std::format("{} seconds", data.get_time_taken().count()), true)
             .add_field("Fridge:",
                        std::format("{}/80", slots_in_fridge),
                        true)
             .add_field("Next completion:",
                        std::format("<t:{}:R>", next_completion), true);

        const auto message = dpp::message(config::discord::channels::info.get(), embed);
        core::discord::bot->message_create(message);
    }
}

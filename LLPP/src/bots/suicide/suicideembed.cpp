#include "suicideembed.h"
#include "../../core/basestation.h"
#include "../../core/discord.h"
#include <format>


namespace llpp::bots::suicide
{
    void send_suicided_embed(const core::StationResult& data,
                             const std::string& at_bed, const std::string& respawning_at)
    {
        auto embed = dpp::embed();
        embed.set_color(dpp::colors::light_blue)
             .set_title(std::format("Successfully suicided at '{}'.", at_bed))
             .set_description(std::format("Suicided a total of {} times!",
                                          data.get_station()->get_times_completed()))
             .set_thumbnail(
                 "https://static.wikia.nocookie.net/"
                 "arksurvivalevolved_gamepedia/images/5/55/"
                 "Specimen_Implant.png/revision/latest?cb=20180731184153")
             .add_field("Time taken:",
                        std::format("{} seconds", data.get_time_taken().count()), true)
             .add_field("Respawning at:", respawning_at, true);

        auto message = dpp::message(
            dpp::snowflake(core::discord::info_channel_id), embed);
        core::discord::bot->message_create(message);
    }
}

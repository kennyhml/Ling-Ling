#include "embeds.h"
#include "../../common/util.h"
#include "../../config/config.h"
#include "../../core/basestation.h"
#include "../../discord/bot.h"
#include "../../discord/helpers.h"

namespace llpp::bots::phoenix
{
    namespace
    {
        const char* const SILICA_PEARL_ICON_URL =
                "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/4/4a/Silica_Pearls.png/revision/latest?cb=20150615130449";

        const char* BLACK_PEARL_ICON_URL =
                "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/4/4c/Black_Pearl.png/revision/latest?cb=20160410175730";
    }

    void send_pearls_collected(const core::StationResult& data)
    {
        const auto next_completion = std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::now() + data.station->
                        get_completion_interval());

        auto embed = dpp::embed();
        embed.set_color(dpp::colors::orange).
                set_title(std::format("Phoenix Station '{}' has been completed.",
                                      data.station->get_name())).
                set_description(std::format(
                "The station was completed successfully {} times!",
                data.station->get_times_completed())).set_thumbnail(SILICA_PEARL_ICON_URL).
                add_field("Time taken:",
                          std::format("{} seconds", data.time_taken.count()),
                          true).add_field(
                "Next completion:", std::format("<t:{}:R>", next_completion), true);

        dpp::message msg(config::discord::channels::info.get(), embed);

        discord::get_bot()->message_create(msg);
    }
}

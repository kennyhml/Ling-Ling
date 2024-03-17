#include "embeds.h"
#include "../../discord/bot.h"
#include "../../common/util.h"
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
            "An enemy player or dinosaur was detected!\n"
            "/silence this station if needed.");
        embed.set_color(dpp::colors::cyan);
        embed.set_thumbnail(SENSOR_ICON);

        const auto file_data = discord::strbuf(asa::window::screenshot());
        embed.set_image("attachment://image.png");

        embed.set_timestamp(std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now()));

        dpp::message msg(config::discord::channels::alert.get(), embed);
        msg.add_file("image.png", file_data, "image/png");
        msg.set_allowed_mentions(false, true, true, false, {}, {});

        switch (tag_level) {
        case 0:
        {
            msg.set_content(
                dpp::utility::role_mention(config::discord::roles::alert.get()));
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

    void send_summary_embed(const std::chrono::seconds time_taken, const std::vector<ParasaurManager::ParasaurGroupStatistics>& stats)
    {
        const auto fmt_taken = std::format("{} seconds", time_taken.count());

        // write list of stations checked
        std::string stations_checked;
        std::string station_icon;
        for(int i = 0; i < static_cast<int>(stats.size()); i++) {
            // check for station name before adding to embed
            if(stats[i].parasaur_station_name != "") {
                if(stats[i].parasaur_alerting) {
                    station_icon = ":alarm_clock:";
                } else {
                    station_icon = ":green_circle:";
                }

                stations_checked += std::format(
                                    "{} {}\n",
                                    station_icon, stats[i].parasaur_station_name
                                    );
            }
        }

        auto embed = dpp::embed().set_color(dpp::colors::cyan).
                                  set_title(std::format(
                                      "Parasaur Stations have been completed!"))
                                  .set_description(
                                      "Here is a summary of the stations checked:")
                                  .set_thumbnail(PARASAUR_THUMBNAIL)
                                  .add_field("", std::format("{}", stations_checked), true)
                                  .add_field("Time taken for all stations:", fmt_taken, true);

        dpp::message msg(config::discord::channels::info.get(), embed);

        discord::get_bot()->message_create(msg);
    }

}

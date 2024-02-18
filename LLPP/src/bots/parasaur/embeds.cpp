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
}

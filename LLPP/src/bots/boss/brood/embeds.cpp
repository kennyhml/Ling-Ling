#include "embeds.h"
#include "../../../common/util.h"
#include "../../../config/config.h"
#include "../../../discord/helpers.h"

namespace llpp::bots::boss
{
    using namespace std::chrono_literals;
    using namespace config::bots::boss;

    namespace
    {
        auto BROODMOTHER_ICON =
                "https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/d/d9/Broodmother_Portal.png/revision/latest?cb=20210812194002";
    }


    dpp::message get_brood_defeated_message(const std::chrono::seconds fight_time,
                                            const std::chrono::seconds total_time,
                                            const cv::Mat& element_loot_image,
                                            const bool element_looted,
                                            const bool element_in_dedi)
    {
        std::string element_where = "Poof :(";
        if (element_looted) {
            element_where = element_in_dedi ? "In Dedi" : "In Rex";
        }
        const std::string time = std::format("{} ({} Fight)",
                                             util::make_counter(total_time),
                                             util::make_counter(fight_time));

        int64_t next = last_brood.get() + std::chrono::seconds(20min).count();

        dpp::embed embed;
        embed.set_title("Broodmother has been defeated!")
             .set_color(0x05f605)
             .set_thumbnail(BROODMOTHER_ICON)
             .set_description("Gamma Broodmother was killed successfully. Details:")
             .add_field("Time:", time, true)
             .add_field("Element:", element_where, true)
             .add_field("Next Fight:", std::format("<t:{}:R>", next), true);

        discord::set_now_timestamp(embed);

        if (element_looted) {
            embed.set_image("attachment://image.png");
        }

        dpp::message message(config::discord::channels::info.get(), embed);

        if (element_looted) {
            const auto filedata = discord::strbuf(element_loot_image);
            message.add_file("image.png", filedata, "image/png");
        }
        return message;
    }
}

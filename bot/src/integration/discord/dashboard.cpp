#include "dashboard.h"
#include "config.h"
#include "discord.h"
#include "commands/common.h"
#include "utility/utility.h"
#include "core/state.h"
#include "core/config.h"

#include <asa/core/logging.h>
#include <asa/utility.h>

#include "asa/core/managedthread.h"

namespace lingling::discord
{
    namespace
    {
        std::vector<std::pair<dashboard_create_listener_t, int32_t> > creation_listeners;

        constexpr auto EMBED_DESCRIPTION =
                "Dynamically updates to reflect the real-time status of the bot.";

        constexpr auto HLNA_ICON_URL =
                "https://ark.wiki.gg/images/3/30/Mini-HLNA_Skin.png";


        [[nodiscard]] std::string get_app_state_repr()
        {
            switch (get_application_state()) {
                case app_state::APP_STATE_ONLINE:
                    return ":blue_circle: - Online";
                case app_state::APP_STATE_PAUSED:
                    return ":yellow_circle: - Paused";
                case app_state::APP_STATE_OFFLINE:
                    return ":red_circle: - Offline";
                case app_state::APP_STATE_RUNNING:
                    return ":green_circle: - Running";
                default:
                    return "???";
            }
        }

        [[nodiscard]] std::string get_active_threads()
        {
            // Make sure we copy the array so we dont run into issues related to threading
            const auto threads = asa::get_all_threads();

            std::string result;
            for (const auto& [name, thread]: threads) {
                std::string state;
                switch (thread->get_state()) {
                    case asa::managed_thread::READY:
                        state = ":blue_circle:";
                        break;
                    case asa::managed_thread::RUNNING:
                        state = ":green_circle:";
                        break;
                    case asa::managed_thread::PAUSED:
                        state = ":yellow_circle:";
                        break;
                    case asa::managed_thread::TERMINATED:
                        state = ":red_circle:";
                        break;
                    default:
                        state = ":question:";
                }
                result += std::format("{} {}\n", state, name);
            }
            return result.empty() ? "`None Active`" : result;
        }

        [[nodiscard]] dpp::embed create_dashboard()
        {
            dpp::embed embed;

            embed.set_title("Ling Ling++ Dashboard")
                 .set_color(dpp::colors::black)
                 .set_thumbnail(HLNA_ICON_URL)
                 .set_description(EMBED_DESCRIPTION);

            const std::string launch_timestamp = std::format(
                "<t:{}:R>", std::chrono::system_clock::to_time_t(get_startup_time()));

            embed.add_field("Application Status:", fmt_field(get_app_state_repr()), true)
                 .add_field("Application Host:", fmt_field(user_name.get()), true)
                 .add_field("Application Startup:", fmt_field(launch_timestamp), true)
                 .add_field("Managed Threads:", fmt_field(get_active_threads()), true)
                 .set_footer({"Last Updated"})
                 .set_timestamp(time(nullptr));

            // Creation listeners can add their own fields to the embed, passed by reference.
            for (const auto& listener: std::ranges::views::keys(creation_listeners)) {
                listener(embed);
            }
            return embed;
        }

        void update_impl(conf_t confirmation)
        {
            if (confirmation.is_error()) {
                asa::get_logger()->error("Dashboard channel update failed: {}",
                                         confirmation.get_error().human_readable);
                return;
            }

            const dpp::message_map map = std::get<dpp::message_map>(confirmation.value);
            auto messages = utility::extract_messages_sorted(map);

            // Delete any message over the first one, we only want to edit a single one.
            for (size_t i = 1; i < messages.size(); i++) {
                get_bot()->message_delete(messages[i].id, messages[i].channel_id);
            }

            // If the message we wanted to edit wasnt sent by the bot we cant edit it.
            if (!messages.empty() && messages[0].author != get_bot()->me) {
                get_bot()->message_delete(messages[0].id, messages[0].channel_id);
                messages.clear(); // All messages are deleted now.
            }

            // If possible edit the existing message - just need to replace the dashboard.
            if (!messages.empty()) {
                messages[0].embeds = {create_dashboard()};
                get_bot()->message_edit(messages[0]);
            } else {
                get_bot()->message_create({dashboard, create_dashboard()});
            }
        }
    }

    void update_dashboard()
    {
        if (dashboard.get().empty()) {
            asa::get_logger()->debug("Dashboard not updated, no channel id provided.");
            return;
        }

        get_bot()->messages_get(dashboard, 0, 0, 0, 100, update_impl);
    }

    std::string fmt_field(const std::string& value)
    {
        return std::format(">>> **{}**", value);
    }

    void add_dashboard_creation_listener(dashboard_create_listener_t fn,
                                         const int32_t position)
    {
        creation_listeners.emplace_back(std::move(fn), position);

        // Sort them by position in ascending order.
        std::ranges::sort(creation_listeners, [](const auto& p1, const auto& p2) {
            return p1.second < p2.second;
        });
    }
}

#include "dashboard.h"
#include <asapp/game/settings.h>
#include <asapp/network/server.h>
#include <asapp/network/queries.h>
#include "bot.h"
#include "helpers.h"
#include "icons.h"
#include "../auth/auth.h"
#include "../common/util.h"
#include "../config/config.h"


namespace llpp::discord
{
    using namespace llpp::config::discord;

    namespace
    {
        std::vector<std::shared_ptr<dpp::embed_field> > dynamic_fields;

        auto session_start = std::chrono::system_clock::now();
        auto last_sv_query = std::chrono::system_clock::time_point();

        std::optional<asa::network::Server> session_server = std::nullopt;

        dpp::message get_previous_dashboard()
        {
            const auto messages = get_bot()->messages_get_sync(
                channels::dashboard.get(), 0, 0, 0, 1);

            if (messages.empty()) {
                std::cout << "[!] No dashboard message found, creating..." << std::endl;
                return {channels::dashboard.get(), dpp::embed()};
            }

            return messages.begin()->second;
        }

        std::string get_server_info()
        {
            if (util::timedout(last_sv_query, std::chrono::minutes(2))) {
                session_server = asa::network::get_server(
                    asa::settings::last_session_3.get());
                last_sv_query = std::chrono::system_clock::now();
            }

            std::string result = "???";
            if (session_server.has_value()) {
                const auto players = std::format(" ({}/70)", session_server->num_players);
                std::string name = session_server->name;
                if (const auto dash = name.find_last_of('-'); dash != std::string::npos) {
                    name = name.substr(dash + 1);
                }
                result = name + players;
            }
            return result;
        }

        dpp::embed create_dashboard_embed()
        {
            dpp::embed embed;
            embed.set_title("Ling Ling++ Dashboard")
                 .set_color(dpp::colors::black)
                 .set_thumbnail(TRANS_ICON)
                 .set_description("Information about the current runtime.");

            const auto duration = std::chrono::system_clock::now() - session_start;

            embed.add_field("Session Host:", auth::user, true)
                 .add_field("Session Time:", util::make_counter(duration), true)
                 .add_field("Session Server:", get_server_info(), true)
                 .add_field("Server Timer", "-:-", true)
                 .add_field("Members Online:", "-/-", true)
                 .add_field("Bot Status:", "IDLE", true);

            for (const auto& field: dynamic_fields) {
                embed.add_field(field->name, field->value, field->is_inline);
            }

            embed.set_footer({"Last Updated"});
            set_now_timestamp(embed);
            return embed;
        }
    }

    void reset_dashboard()
    {
        session_start = std::chrono::system_clock::now();
        dynamic_fields.clear();
    }

    void
    update_dashboard()
    {
        auto msg = get_previous_dashboard();
        msg.embeds.clear();
        msg.add_embed(create_dashboard_embed());

        // Create the message if we need to
        if (msg.id) { get_bot()->message_edit_sync(msg); }
        else { get_bot()->message_create_sync(msg); }
    }

    void register_dashboard_field(std::shared_ptr<dpp::embed_field> field)
    {
        dynamic_fields.emplace_back(std::move(field));
    }
}

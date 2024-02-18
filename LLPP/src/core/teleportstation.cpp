#include "teleportstation.h"
#include "../discord/embeds.h"
#include "../discord/bot.h"
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/spawnmap.h>
#include "../config/config.h"

namespace llpp::core
{
    namespace
    {
        void send_disabled(const std::string& name, const std::string& why)
        {
            dpp::message msg(discord::get_error_channel(),
                     discord::get_station_disabled_embed(name, why));

            msg.set_content(
                dpp::utility::role_mention(
                    config::discord::roles::helper_no_access.get()));
            msg.set_allowed_mentions(false, true, false, false, {}, {});

            discord::get_bot()->message_create(msg);
        }
    }

    TeleportStation::TeleportStation(std::string t_name,
                                     const std::chrono::minutes t_interval)
        : BaseStation(std::move(t_name), t_interval), start_tp_(name_) {}

    bool TeleportStation::begin()
    {
        last_started_ = std::chrono::system_clock::now();

        try {
            asa::entities::local_player->teleport_to(start_tp_, is_default_);
        } catch (const asa::interfaces::DestinationNotFound& e) {
            std::cerr << e.what() << std::endl;
            send_disabled(name_, e.what());
            set_state(State::DISABLED);
            return false;
        }
        return true;
    }
}

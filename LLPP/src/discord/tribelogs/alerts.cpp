#include "alerts.h"
#include "../../config/config.h"
#include "../icons.h"
#include "../bot.h"
#include "../../common/util.h"

namespace
{
    using asa::interfaces::components::TribeLogMessage;

    bool suicided_or_starved(const TribeLogMessage& msg)
    {
        // only care for tribe mates and tribe tames dying
        if (msg.type != TribeLogMessage::TRIBE_PLAYER_KILLED && msg.type !=
            TribeLogMessage::TRIBE_DINO_KILLED) { return false; }

        return msg.content.find("by") != std::string::npos;
    }

    const char* get_event_title(const TribeLogMessage::EventType event)
    {
        switch (event) {
        case TribeLogMessage::TRIBE_DINO_KILLED: return "A Tame was killed!";
        case TribeLogMessage::TRIBE_PLAYER_KILLED: return "A Tribemember was killed!";
        case TribeLogMessage::TRIBE_DESTROYED: return "Something was destroyed!";
        case TribeLogMessage::ENEMY_DINO_KILLED: return "Killed a dinosaur!";
        case TribeLogMessage::ENEMY_PLAYER_KILLED: return "Killed an enemy player!";
        default:
            return nullptr;
        }
    }
}

namespace llpp::discord
{
    using namespace config::discord::alert_rules;

    namespace
    {
        std::chrono::system_clock::time_point last_ping;

        const std::vector<std::string> ALWAYS_IGNORE{"Anti-meshing", "decay", "Baby"};

        const char* get_event_icon(const TribeLogMessage::EventType event)
        {
            switch (event) {
            case TribeLogMessage::TRIBE_DINO_KILLED: return DINO_ICON;
            case TribeLogMessage::TRIBE_PLAYER_KILLED: return SKULL_WHITE_ICON;
            case TribeLogMessage::TRIBE_DESTROYED: return C4_ICON;
            case TribeLogMessage::ENEMY_DINO_KILLED: return TURRET_ICON;
            case TribeLogMessage::ENEMY_PLAYER_KILLED: return SKULL_RED_ICON;
            default:
                return "";
            }
        }

        bool has_ignored_term(const TribeLogMessage& msg)
        {
            auto ignored = ALWAYS_IGNORE;
            for (const auto& term : ignore_filter.get()) { ignored.emplace_back(term); }
            return std::ranges::any_of(ignored, [&msg](const std::string& term) -> bool {
                return msg.content.find(term) != std::string::npos;
            });
        }

        bool has_alert_term(const TribeLogMessage& msg)
        {
            auto alert = ping_filter.get();
            return std::ranges::any_of(alert, [&msg](const std::string& term) -> bool {
                return msg.content.find(term) != std::string::npos;
            });
        }

        bool can_ping()
        {
            return util::timedout(last_ping, std::chrono::seconds(ping_cooldown.get()));
        }

        dpp::embed get_alert_embed(const TribeLogMessage& msg)
        {
            dpp::embed embed;
            embed.set_title(get_event_title(msg.type));
            embed.set_color(0xFF0000);
            embed.set_description(msg.timestamp.to_string());

            embed.add_field(msg.content, "");
            embed.set_timestamp(std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::now()));
            return embed;
        }
    }

    void handle_alerts(const asa::interfaces::TribeManager::LogEntries& events)
    {
        dpp::message message(config::discord::channels::alert.get(), "");
        bool tag_everyone = false;

        for (const auto& event : events) {
            if (has_ignored_term(event) || suicided_or_starved(event)) { continue; }

            const char* icon_url = get_event_icon(event.type);
            if (!*icon_url) { continue; } // not an event type we care to alert for
            tag_everyone |= has_alert_term(event);

            message.embeds.emplace_back(get_alert_embed(event).set_thumbnail(icon_url));
        }
        // handle the case where every single event was ignored.
        if (message.embeds.empty()) { return; }

        if (can_ping() && message.embeds.size() >= ping_min_events.get()) {
            if (tag_everyone) { message.set_content("@everyone"); }
            else {
                const std::string role = config::discord::roles::alert.get();
                message.set_content(dpp::utility::role_mention(role));
            }
        }
        message.set_allowed_mentions(false, true, true, false, {}, {});
        get_bot()->message_create(message);
    }
}

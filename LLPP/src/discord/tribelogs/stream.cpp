#include "stream.h"
#include "../bot.h"
#include "../../config/config.h"
#include <ranges>

namespace
{
    using asa::interfaces::components::TribeLogMessage;

    enum AnsiTextColor_
    {
        TEXT_GRAY = 30,
        TEXT_RED,
        TEXT_GREEN,
        TEXT_YELLOW,
        TEXT_BLUE,
        TEXT_PINK,
        TEXT_CYAN,
        TEXT_WHITE,
    };

    AnsiTextColor_ get_text_color(const TribeLogMessage::EventType event)
    {
        switch (event) {
        case TribeLogMessage::EventType::TRIBE_DESTROYED:
        case TribeLogMessage::EventType::TRIBE_PLAYER_KILLED:
        case TribeLogMessage::EventType::TRIBE_DINO_KILLED:
        case TribeLogMessage::EventType::DEMOTED:
        case TribeLogMessage::EventType::PLAYER_REMOVED:
            return TEXT_RED;
        case TribeLogMessage::EventType::DINO_CRYOD:
        case TribeLogMessage::EventType::DINO_STARVED:
        case TribeLogMessage::EventType::TRIBE_GROUP_UPDATED:
            return TEXT_WHITE;
        case TribeLogMessage::EventType::ENEMY_DINO_KILLED:
        case TribeLogMessage::EventType::ENEMY_PLAYER_KILLED:
        case TribeLogMessage::EventType::CLAIMED:
            return TEXT_PINK;
        case TribeLogMessage::EventType::DEMOLISHED:
        case TribeLogMessage::EventType::UNCLAIMED:
        case TribeLogMessage::EventType::ENEMY_DESTROYED:
            return TEXT_YELLOW;
        case TribeLogMessage::EventType::PROMOTED:
            return TEXT_BLUE;
        case TribeLogMessage::EventType::PLAYER_ADDED:
            return TEXT_CYAN;
        case TribeLogMessage::EventType::DINO_TAMED:
            return TEXT_GREEN;
        default:
            return TEXT_GRAY;
        }
    }

    bool could_have_turret_name(const TribeLogMessage::EventType event_type)
    {
        return event_type == TribeLogMessage::ENEMY_PLAYER_KILLED || event_type ==
            TribeLogMessage::ENEMY_DINO_KILLED;
    }

    std::string fmt(const TribeLogMessage& event)
    {
        if (could_have_turret_name(event.type)) {
            // if we find )! (, there is a turret name.
            if (const size_t p = event.content.find(")! ("); p != std::string::npos) {
                const std::string killed = event.content.substr(0, p + 3);
                const std::string where = event.content.substr(p + 3);
                return std::format("{}: \u001b[1;35m{}\u001b[0m{}\n",
                                   event.timestamp.to_string(), killed, where);
            }
        }
        return std::format("{}: \u001b[1;{}m{}\u001b[0m\n", event.timestamp.to_string(),
                           static_cast<int>(get_text_color(event.type)), event.content);
    }

    std::vector<std::string> fmt_all(const asa::interfaces::TribeManager::LogEntries& log)
    {
        std::vector<std::string> result;
        for (const auto& event : std::ranges::views::reverse(log)) {
            result.push_back(fmt(event));
        }
        return result;
    }
}

namespace llpp::discord
{
    void handle_stream(const asa::interfaces::TribeManager::LogEntries& entries,
                       const bool flush_previous_messages)
    {
        const std::vector<std::string> fmt_messages = fmt_all(entries);

        std::vector<dpp::message> messages;

        const dpp::command_completion_event_t callback = [fmt_messages,
                flush_previous_messages
            ](const dpp::confirmation_callback_t& confirmation)-> void {
            if (confirmation.is_error()) {
                std::cerr << "[!] messages_get failed: " << confirmation.get_error().
                    message << std::endl;
                return;
            }

            std::vector<dpp::snowflake> msg_ids;
            const dpp::message_map map = std::get<dpp::message_map>(confirmation.value);

            int message_iter = 0;

            dpp::message most_recent;
            double latest_found = 0;

            for (const auto& [id, message] : map) {
                msg_ids.push_back(id);
                if (message.get_creation_time() > latest_found) {
                    most_recent = message;
                    latest_found = message.get_creation_time();
                }
            }

            if (!(flush_previous_messages || msg_ids.empty()) && most_recent.author.id ==
                get_bot()->me.id) {
                std::string original_content = most_recent.content;

                while (most_recent.content.length() < 1900) {
                    if (message_iter >= fmt_messages.size()) { break; }

                    const std::string& line = fmt_messages[message_iter++];
                    most_recent.content.insert(most_recent.content.length() - 3, line);
                }
                if (message_iter) { discord::get_bot()->message_edit(most_recent); }
            }
            else if (flush_previous_messages) {
                const auto channel = config::discord::channels::logs.get();
                if (msg_ids.size() == 1) {
                    discord::get_bot()->message_delete(msg_ids[0], channel);
                }
                else if (!msg_ids.empty()) {
                    discord::get_bot()->message_delete_bulk(msg_ids, channel);
                }
            }

            dpp::message new_message(config::discord::channels::logs.get(), "```ansi\n");
            while (message_iter < fmt_messages.size()) {
                if (new_message.content.length() > 1900) {
                    new_message.content += "```";
                    discord::get_bot()->message_create(new_message);
                    new_message.content = "```ansi\n";
                }
                new_message.content += fmt_messages[message_iter++];
            }
            new_message.content += "```";
            if (new_message.content.length() > 15) {
                get_bot()->message_create(new_message);
            }
        };

        get_bot()->messages_get(config::discord::channels::logs.get(), 0, 0, 0, 100,
                                callback);
    }
}

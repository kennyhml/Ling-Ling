#include "discord.h"
#include "../common/util.h"
#include <asapp/game/window.h>

#include "../config/config.h"

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

enum AnsiBackgroundColor_
{
    BACKGROUND_FIREFLY_DARK_BLUE = 40,
    BACKGROUND_ORANGE,
    BACKGROUND_MARBLE_BLUE,
    BACKGROUND_GREYISH_TURQUOISE,
    BACKGROUND_GRAY,
    BACKGROUND_INDIGO,
    BACKGROUND_LIGHT_GRAY,
    BACKGROUND_WHITE
};

namespace
{
    using asa::interfaces::components::TribeLogMessage;

    AnsiTextColor_ get_text_color_for(const TribeLogMessage::EventType event)
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

    std::vector<std::string> format(const asa::interfaces::TribeManager::LogEntries& log)
    {
        std::vector<std::string> result;
        for (int i = log.size() - 1; i >= 0; i--) {
            switch (log[i].event) {
            case TribeLogMessage::EventType::ENEMY_DINO_KILLED:
            case TribeLogMessage::EventType::ENEMY_PLAYER_KILLED:
            {
                // if we find )! (, there is a turret name.
                if (size_t pos = log[i].content.find(")! ("); pos != std::string::npos) {
                    std::string killed = log[i].content.substr(0, pos + 3);
                    std::string where = log[i].content.substr(pos + 3);

                    result.push_back(std::format("{}: \u001b[1;35m{}\u001b[0m{}\n",
                                                 log[i].timestamp.to_string(), killed,
                                                 where));
                    break;
                }
            }
            default:
            {
                int color_code = get_text_color_for(log[i].event);
                result.push_back(std::format("{}: \u001b[1;{}m{}\u001b[0m\n",
                                             log[i].timestamp.to_string(), color_code,
                                             log[i].content));
            }
            }
        }
        return result;

    }
}

namespace llpp::core::discord
{
    std::unordered_map<std::string, event_callback_t> event_callbacks;
    std::vector<dpp::slashcommand> commands;

    namespace
    {
        bool is_initialized = false;

        void validate_configuration()
        {
            using namespace config::discord;
            if (token.get_ptr()->empty()) {
                throw config::BadConfigurationError(
                    "Bad configuration: Bot token must be a valid token, not empty.");
            }

            if (guild.get_ptr()->empty()) {
                throw config::BadConfigurationError(
                    "Bad configuration: Guild ID must not be empty.");
            }

            if (channels::info.get_ptr()->empty()) {
                throw config::BadConfigurationError(
                    "Bad configuration: Info channel id must not be empty.");
            }

            if (roles::helper_no_access.get_ptr()->empty() || roles::helper_access.
                get_ptr()->empty()) {
                throw config::BadConfigurationError(
                    "Bad configuration: Helper role ids must not be empty.");
            }
        }

        void ready_callback(const dpp::ready_t& event)
        {
            if (dpp::run_once<struct register_bot_commands>()) {
                std::cout << "[+] Registering discord slash commands... ";

                for (auto& cmd : commands) { cmd.application_id = bot->me.id; }

                bot->guild_bulk_command_create_sync(commands,
                                                    config::discord::guild.get());
                std::cout << "Done.\n";
            }

            is_initialized = true;
        }

        void slashcommand_callback(const dpp::slashcommand_t& event)
        {
            auto& fn = event_callbacks.at(event.command.get_command_name());
            fn(event);
        }
    }

    bool init()
    {
        std::cout << "[+] Initializing Ling Ling++ discord bot...\n";
        validate_configuration();

        std::cout << "\t[-] Creating bot from token... ";
        bot = std::make_unique<dpp::cluster>(config::discord::token.get(),
                                             dpp::i_all_intents);
        std::cout << "Done\n";

        std::cout << "\t[-] Registering static callbacks... ";
        bot->on_slashcommand(slashcommand_callback);
        bot->on_ready(ready_callback);
        bot->on_log(dpp::utility::cout_logger());
        std::cout << "Done\n";

        std::cout << "\t[-] Discord bot created successfully.\n";
        return true;
    }

    bool handle_unauthorized_command(const dpp::slashcommand_t& event)
    {
        if (!is_channel_command_authorized(event.command.channel_id)) {
            event.reply(
                dpp::message("You can't use Ling Ling++ commands in this channel.").
                set_flags(dpp::m_ephemeral));
            return true;
        }
        if (!is_user_command_authorized(event.command.member)) {
            event.reply(
                dpp::message("You are not authorized to use commands.").set_flags(
                    dpp::m_ephemeral));
            return true;
        }
        return false;
    }

    bool is_user_command_authorized(const dpp::guild_member& user)
    {
        using namespace config::discord::authorization;

        // no roles specified, allow any user to use commands.
        if (users.get_ptr()->empty() && roles.get_ptr()->empty()) { return true; }

        // check if the user has any of the authorized roles
        if (std::ranges::any_of(user.get_roles(), is_role_command_authorized)) {
            return true;
        }

        // check if the user itself is authorized
        const auto allowed = users.get_ptr();
        return std::ranges::find(*allowed, user.user_id.str()) != allowed->end();
    }

    bool is_channel_command_authorized(const dpp::snowflake channel)
    {
        using namespace config::discord::authorization;

        return channels.get_ptr()->empty() || std::ranges::find(
            *channels.get_ptr(), channel.str()) != channels.get_ptr()->end();
    }

    bool is_role_command_authorized(const dpp::snowflake role)
    {
        const auto allowed = config::discord::authorization::roles.get_ptr();
        return std::ranges::find(*allowed, role.str()) != allowed->end();
    }

    void register_slash_command(const dpp::slashcommand& cmd,
                                const event_callback_t& callback)
    {
        std::cout << "[+] Registering slash command: " << cmd.name << "... ";
        commands.push_back(cmd);
        event_callbacks[cmd.name] = callback;
        std::cout << " Done\n";
    }

    void inform_started()
    {
        auto started_embed = dpp::embed().set_title("Ling Ling++ is starting...").
                                          set_description(std::format(
                                              "Ling Ling++ has been started at <t:{}:t>",
                                              std::chrono::system_clock::to_time_t(
                                                  std::chrono::system_clock::now()))).
                                          set_color(dpp::colors::black).
                                          add_field("Account: ", config::user::name.get(),
                                                    true).add_field(
                                              "Server: ",
                                              config::general::ark::server.get(),
                                              true).set_thumbnail(
                                              "https://static.wikia.nocookie.net/"
                                              "arksurvivalevolved_gamepedia/images/b/b9/"
                                              "Whip_%28Scorched_Earth%29.png/revision/latest/"
                                              "scale-to-width-down/228?cb=20160901213011");

        bot->message_create(dpp::message(config::discord::channels::info.get(),
                                         started_embed));
    }

    void inform_fatal_error(const std::exception& error, const std::string& task)
    {
        dpp::embed embed = dpp::embed().set_title("FATAL! Ling Ling++ has crashed!").
                                        set_description(
                                            "Encountered an unexpected error - manual "
                                            "restart required.").
                                        set_color(dpp::colors::red).
                                        add_field("Error: ", error.what(), true).
                                        add_field("During Task: ", task, true).
                                        set_thumbnail(
                                            "https://static.wikia.nocookie.net/"
                                            "arksurvivalevolved_gamepedia/images/1/1c/"
                                            "Repair_Icon.png/revision/latest?cb=20150731134649")
                                        .set_image("attachment://image.png");

        const auto file_data = util::mat_to_strbuffer(asa::window::screenshot());
        dpp::message message = dpp::message(config::discord::channels::info.get(),
                                            std::format(
                                                "<@&{}>",
                                                config::discord::roles::helper_access.
                                                get())).set_allowed_mentions(
            false, true, false, false, {}, {});
        message.add_file("image.png", file_data, "image/png ").add_embed(embed);

        bot->message_create(message);
    }

    void handle_tribelogs(const asa::interfaces::TribeManager::LogEntries& all_,
                          const asa::interfaces::TribeManager::LogEntries& new_)
    {
        // Initialize here to avoid the configuration changing mid function.
        // If we flush the previous messages we want all events, otherwise we
        // only care about the new ones.
        const bool flush = config::discord::advanced::flush_logs.get();
        const std::vector<std::string> fmt_messages = format(flush ? all_ : new_);

        const auto channel = config::discord::channels::logs.get();
        std::vector<dpp::message> messages;

        const dpp::command_completion_event_t callback = [fmt_messages
            ](const dpp::confirmation_callback_t& confirmation)-> void {
            std::vector<dpp::snowflake> msg_ids;
            const dpp::message_map map = std::get<dpp::message_map>(confirmation.value);

            int message_iter = 0;
            for (const auto& id : std::views::keys(map)) { msg_ids.push_back(id); }

            if (!config::discord::advanced::flush_logs.get() && !msg_ids.empty()) {
                auto last_msg = map.at(msg_ids.front());

                std::string original_content = last_msg.content;

                while (last_msg.content.length() < 1900) {
                    if (message_iter >= fmt_messages.size()) { break; }

                    const std::string& line = fmt_messages[message_iter++];
                    last_msg.content.insert(last_msg.content.length() - 3, line);
                }
                if (message_iter) { bot->message_edit(last_msg); }
            }
            else if (config::discord::advanced::flush_logs.get()) {
                const auto channel = config::discord::channels::logs.get();
                if (msg_ids.size() == 1) { bot->message_delete(msg_ids[0], channel); }
                else if (!msg_ids.empty()) { bot->message_delete_bulk(msg_ids, channel); }
            }

            dpp::message new_message(config::discord::channels::logs.get(), "```ansi\n");
            while (message_iter < fmt_messages.size()) {
                if (new_message.content.length() > 1900) {
                    new_message.content += "```";
                    bot->message_create(new_message);
                    new_message.content = "```ansi\n";
                }
                new_message.content += fmt_messages[message_iter++];
            }
            new_message.content += "```";
            if (new_message.content.length() > 15) { bot->message_create(new_message); }
        };

        bot->messages_get(channel, 0, 0, 0, 100, callback);
    }
}

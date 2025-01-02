#include "discord.h"
#include "config.h"
#include "queue.h"
#include "discord/format.h"
#include "discord/discord.h"

#include <magic_enum.hpp>
#include <asa/core/logging.h>

namespace lingling
{
    namespace
    {
        const std::vector<discord::discord_configurable> CONFIG_CHOICES
        {
            {"Task Queue Channel", "channel", dpp::co_channel, "The new channel."},
        };

        const managed_var_table_t<dpp::snowflake> channel_vars = {
            {"Task Queue Channel", q_channel},
        };

        discord::ansi_color prio_color(const task_priority priority)
        {
            switch (priority) {
                case task_priority::PRIORITY_CRITICAL:
                    return discord::ANSI_COLOR_RED;
                case task_priority::PRIORITY_HIGH:
                    return discord::ANSI_COLOR_YELLOW;
                case task_priority::PRIORITY_MEDIUM:
                    return discord::ANSI_COLOR_BLUE;
                default:
                    return discord::ANSI_COLOR_WHITE;
            }
        }

        std::vector<dpp::message> get_task_queue_table()
        {
            // Dont take by reference since we iterate over it!
            const auto task_queue = get_all_tasks_in_queue();

            constexpr discord::ansi_color header_color = discord::ANSI_COLOR_CYAN;
            constexpr discord::ansi_style header_style = discord::ANSI_STYLE_BOLD;

            // First passthrough to find the biggest width of each column
            std::array<size_t, 4> widths{8, 6, 7, 11};
            for (const auto& t: task_queue) {
                std::string prio = magic_enum::enum_name(t->get_priority()).data();
                widths[0] = std::max(widths[0], prio.length());
                widths[1] = std::max(widths[1], t->get_module().length());
                widths[2] = std::max(widths[2], t->get_id().length());
                widths[3] = std::max(widths[3], t->get_description().length());
            }

            std::vector<discord::text_row_t> rows;
            for (const auto& t: task_queue) {
                discord::text_row_t row;

                // Always color priority color
                row.emplace_back(magic_enum::enum_name(t->get_priority()).data(),
                                 widths[0],
                                 prio_color(t->get_priority()), discord::ANSI_STYLE_BOLD);

                row.emplace_back(t->get_module(), widths[1]);
                row.emplace_back(t->get_id(), widths[2]);
                row.emplace_back(t->get_description(), widths[3]);
                rows.emplace_back(row);
            }

            const discord::text_row_t header = {
                {"Priority", static_cast<int32_t>(widths[0]), header_color, header_style},
                {"Module", static_cast<int32_t>(widths[1]), header_color, header_style},
                {"Task ID", static_cast<int32_t>(widths[2]), header_color, header_style},
                {
                    "Description", static_cast<int32_t>(widths[3]), header_color,
                    header_style
                },
            };
            rows.insert(rows.begin(), header);

            auto ret = table(rows);

            // Set the channel ID, the table function doesnt know that!
            ret[0].content.insert(0, std::format("**Updated <t:{}:R>**.", time(nullptr)));
            for (auto& msg: ret) { msg.channel_id = q_channel; }
            return ret;
        }

        void update_impl(discord::conf_t confirmation)
        {
            if (confirmation.is_error()) {
                asa::get_logger()->error("Task Queue channel update failed: {}",
                                         confirmation.get_error().human_readable);
                return;
            }

            const dpp::message_map map = std::get<dpp::message_map>(confirmation.value);

            std::vector<dpp::message> msg_old;
            msg_old.reserve(map.size());
            std::ranges::transform(map, std::back_inserter(msg_old), [](const auto& p) {
                return p.second;
            });
            // Sort by creation time in ascending order (older messages first)
            std::ranges::sort(msg_old, [](const auto& m1, const auto& m2) {
                return m1.get_creation_time() < m2.get_creation_time();
            });

            std::vector<dpp::message> msg_new = get_task_queue_table();

            // Check if ANY new message has to be posted, discord makes an ugly seperation
            // between messages when the time between them is too long. In this case just
            // delete all previous messages.
            if (msg_new.size() > msg_old.size()) {
                for (const auto& msg: msg_old) {
                    discord::get_bot()->message_delete(msg.id, msg.channel_id);
                }
                for (const auto& msg: msg_new) {
                    discord::get_bot()->message_create(msg);
                }
                return;
            }

            // Edit the existing messages, we just need to flip the ID.
            for (size_t i = 0; i < msg_new.size(); i++) {
                msg_new[i].id = msg_old[i].id;
                discord::get_bot()->message_edit(msg_new[i]);
            }

            // Delete any excess messages
            for (size_t i = msg_new.size(); i < msg_old.size(); i++) {
                discord::get_bot()->message_delete(msg_old[i].id, msg_old[i].channel_id);
            }
        }

        void handle_config_command(const dpp::slashcommand_t& event)
        {
            const auto var = discord::tget<std::string>(event.get_parameter("variable"));
            if (channel_vars.contains(var)) {
                discord::handle_change(event, var, "channel", channel_vars.at(var));
            } else {
                event.reply(std::format("Variable `{}` is unknown!", var));
            }
        }
    }

    void update_task_queue_channel()
    {
        if (q_channel.get().empty()) {
            asa::get_logger()->debug("Task Queue channel not updated - no channel ID.");
            return;
        }

        discord::get_bot()->messages_get(q_channel, 0, 0, 0, 100, update_impl);
    }

    discord::command_callback_t add_config_tasksystem_command_group(
        dpp::slashcommand& command)
    {
        dpp::command_option group(dpp::co_sub_command_group, "tasksystem",
                                  "Provides control over Ling-Lings Tasksystem.");

        group.add_option(discord::create_config_ctrl_command("set", CONFIG_CHOICES,
            "Change a variable in the tasksystem configuration."));

        group.add_option(discord::create_config_ctrl_command("get", CONFIG_CHOICES,
            "Get the value of a variable in the tasksystem configuration.", false));

        command.add_option(group);
        return handle_config_command;
    }
}

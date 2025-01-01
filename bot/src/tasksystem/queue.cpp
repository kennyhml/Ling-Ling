#include "queue.h"
#include "integration/discord/discord.h"

#include <asa/core/managedthread.h>
#include <asa/core/state.h>
#include <asa/core/logging.h>
#include <magic_enum.hpp>
#include <array>

#include "integration/discord/format.h"


namespace lingling
{
    namespace
    {
        constexpr auto TASK_READINESS_FETCH_INTERVAL = 5s;

        std::vector<task_enqueue_lookup_callback_t> callbacks;
        std::deque<std::shared_ptr<task> > task_queue;
        std::mutex queue_mutex;


        discord::ansi_color get_color_for(const task_priority priority)
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
    }

    std::shared_ptr<task> get_next_task_in_queue()
    {
        std::lock_guard lock(queue_mutex);
        if (task_queue.empty()) { return nullptr; }

        std::shared_ptr<task> ret = task_queue.front();
        task_queue.pop_front();
        return ret;
    }

    const task* peek_next_task_in_queue()
    {
        std::lock_guard lock(queue_mutex);
        if (task_queue.empty()) { return nullptr; }

        return task_queue.front().get();
    }

    const task_queue_t& get_all_tasks_in_queue()
    {
        return task_queue;
    }

    void enqueue_task(const std::shared_ptr<task>& new_task)
    {
        asa::get_logger()->info("Enqueuing task '{}' ({})..", new_task->get_id(),
                                new_task->get_module());
        // Make sure the state of the task matches the fact that we are enqueuing it
        if (new_task->get_state() != task_state::STATE_WAITING) {
            asa::get_logger()->warn("Task state was not STATE_WAITING, enqueue skipped!");
            return;
        }
        std::lock_guard lock(queue_mutex);

        for (auto it = task_queue.begin(); it != task_queue.end(); ++it) {
            // Insert the new task before the first task with a lower or equal priority
            if ((*it)->get_priority() < new_task->get_priority()) {
                task_queue.insert(it, new_task);
                new_task->set_state(task_state::STATE_ENQUEUED);
                return;
            }
        }

        // Didnt insert the task then the queue is either empty or all tasks in the queue
        // had a higher priority, in either case it is fine to add it to the back.
        task_queue.emplace_back(new_task);
        new_task->set_state(task_state::STATE_ENQUEUED);
    }

    void dequeue_task(const std::shared_ptr<task>& erase_task)
    {
        std::lock_guard lock(queue_mutex);

        // Erase the task that matches the module and id of the provided task.
        std::erase_if(task_queue, [&erase_task](const std::shared_ptr<task>& t) {
            return t->get_module() == erase_task->get_module() &&
                   t->get_id() == erase_task->get_id();
        });
    }

    void add_task_enqueue_lookup_listener(task_enqueue_lookup_callback_t fn)
    {
        callbacks.push_back(std::move(fn));
    }

    void start_queue_handler_thread()
    {
        asa::register_thread("Taskqueue Handler", [] {
            // Go over the registered callbacks every 5 seconds.
            for (const auto& fn: callbacks) {
                if (std::shared_ptr<task> task = fn(); task != nullptr) {
                    enqueue_task(task);
                }
            }
            asa::checked_sleep(TASK_READINESS_FETCH_INTERVAL);
        });
        asa::get_thread("Taskqueue Handler")->start();
    }

    const std::string& get_taskqueue_view()
    {
        std::lock_guard lock(queue_mutex);

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
            row.emplace_back(magic_enum::enum_name(t->get_priority()).data(), widths[0],
                             get_color_for(t->get_priority()), discord::ANSI_STYLE_BOLD);

            row.emplace_back(t->get_module(), widths[1]);
            row.emplace_back(t->get_id(), widths[2]);
            row.emplace_back(t->get_description(), widths[3]);
            rows.emplace_back(row);
        }

        const discord::text_row_t header = {
            {"Priority", static_cast<int32_t>(widths[0]), header_color, header_style},
            {"Module", static_cast<int32_t>(widths[1]), header_color, header_style},
            {"Task ID", static_cast<int32_t>(widths[2]), header_color, header_style},
            {"Description", static_cast<int32_t>(widths[3]), header_color, header_style},
        };
        rows.insert(rows.begin(), header);

        for (auto& msg: table(rows)) {
            msg.channel_id = dpp::snowflake(1144360787029278740);
            discord::get_bot()->message_create(msg);
        }

        return "";
    }
}

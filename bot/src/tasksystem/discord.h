#pragma once
#include "discord/commands/config.h"

namespace lingling
{
    /**
     * @brief Updates the task queue channel with a visualization of the updated task queue.
     * If there are already messages in the channel, they are edited however possible to avoid
     * deleting / reposting messages.
     *
     * @remark If there is no channel ID set, the task queue will not be posted.
     */
    void update_task_queue_channel();

    /**
     * @brief Registers the /config tasksystem subcommand to allow changing the tasksystem
     * configuration through discord.
     *
     * @return The function to will handle the /config tasksystem command being invoked.
     */
    discord::command_callback_t add_slashcommand_subgroup(dpp::slashcommand&);
}

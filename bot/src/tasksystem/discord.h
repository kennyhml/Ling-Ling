#pragma once
#include "discord/commands/config.h"

namespace lingling
{
    void update_task_queue_channel();

    discord::command_callback_t add_slashcommand_subgroup(dpp::slashcommand&);
}

#include "tasksystem.h"

#include "queue.h"
#include "configuration/validate.h"
#include "discord/commands/config.h"

namespace lingling
{
    void init_tasksystem()
    {
        // Add the config validation
        add_config_validation("tasksystem", validate_tasksystem_config);

        // Add the listener to the /config command to register /config tasksystem group.
        discord::add_config_command_create_listener(add_config_tasksystem_command_group);

        // Update the discord task queue when the task queue is updated
        add_task_queue_updated_listener(update_task_queue_channel);
    }
}

#include "tasksystem.h"
#include "configuration/validate.h"
#include "discord/commands/config.h"

namespace lingling
{
    void init_tasksystem()
    {
        // Add the config validation
        add_config_validation("tasksystem", validate_tasksystem_config);

        // Add the listener to the /config command to register /config tasksystem group.
        discord::add_config_command_create_listener(add_slashcommand_subgroup);
    }
}

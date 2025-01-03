#pragma once
#include "discord/commands/config.h"

namespace lingling::gacha
{
    /**
     * @brief Adds the /config gacha subcommand group to the /config commands. It allows:
     *
     * - CRUD operations on gacha tower presets.
     * - CRUD operations on individual tasks of tower presets.
     * - Changing the active tower preset.
     *
     * @param cmd The command to register the gacha command group to.
     *
     * @return The function to reroute any commands invoking this the subcommand to.
     */
    discord::command_callback_t add_config_gacha_command_group(dpp::slashcommand& cmd);
}

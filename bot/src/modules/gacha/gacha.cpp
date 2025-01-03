#include "gacha.h"
#include "config.h"
#include "discord.h"
#include "configuration/validate.h"
#include "discord/commands/config.h"

namespace lingling::gacha
{
    void initialize_module()
    {
        // Make sure the gacha configs get validated, this is a little bit more trouble
        // since we have a dynamic number of tower presets + global config.
        add_config_validation("gacha", validate_gacha_config);

        // Add the gacha subcommand group configuration
        discord::add_config_command_create_listener(add_config_gacha_command_group);
    }
}

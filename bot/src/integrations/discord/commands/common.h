#pragma once
#include <dpp/dpp.h>

namespace discord
{
    // Callback function type for a slashcommand event
    using slashcommand_callback_t = std::function<void(const dpp::slashcommand_t&)>;

    // Slashcommand and the callback to handle the command being triggered
    using slashcommand_register_t = std::pair<dpp::slashcommand, slashcommand_callback_t>;

    // Callback function to attach a subcommand with a callback to handle it being triggered
    using command_create_t = std::function<slashcommand_callback_t(dpp::slashcommand&)>;
}

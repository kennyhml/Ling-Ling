#pragma once
#include "core/startup.h"
#include <dpp/dpp.h>

namespace lingling::discord
{
    /**
     * @brief Initializes the discord bot, at this time the bot is authenticated with
     * the token, the guild is obtained and slashcommands are registered.
     */
    void initialize_discord_bot();

    [[nodiscard]] std::shared_ptr<dpp::cluster> get_bot();

    // Initialize the discord bot on late startup, has to happen in the late phase because
    // the configuration is only validated / created in the phase before.
    inline run_on_startup on_startup(startup_time::STARTUP_LATE, [] {
        initialize_discord_bot();
    });
}

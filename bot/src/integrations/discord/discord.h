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

    inline run_on_startup on_startup([] {
        initialize_discord_bot();
    }, false);
}

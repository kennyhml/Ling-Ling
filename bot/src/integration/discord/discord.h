#pragma once
#include "core/startup.h"
#include <dpp/dpp.h>

namespace lingling::discord
{
    /**
     * @brief Initializes the discord module, allows the required callbacks to be set.
     */
    void module_initialize();

    /**
     * @brief Initializes the discord bot, at this time the bot is authenticated with
     * the token, the guild is obtained and slashcommands are registered.
     */
    void initialize_discord_bot();

    void update_command_list();

    /**
     * @brief Gets a shared pointer to the dpp::cluster serving as the bot. May be nullptr
     * if the bot is not initialized.
     */
    [[nodiscard]] std::shared_ptr<dpp::cluster> get_bot();

    inline run_on_startup module_init(startup_time::STARTUP_EARLY, [] {
        module_initialize();
    });
}

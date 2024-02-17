#pragma once
#include <dpp/dpp.h>

namespace llpp::discord
{
    using event_callback_t = std::function<void(const dpp::slashcommand_t&)>;

    /**
     * @brief Initializes the discord bot with the entered configuration.
     *
     * @return True if successful, false otherwise.
     */
    bool init();

    /**
     * @brief Returns a pointer to the current dpp::cluster bot instance.
     */
    dpp::cluster* get_bot();

    /**
     * @brief Checks whether a command and the channel it was sent in are valid.
     *
     * @param event The slashcommand event to check whether it is valid.
     *
     * @return True if the command is unauthorized and was handled, false otherwise.
     */
    bool handle_unauthorized_command(const dpp::slashcommand_t& event);

    /**
     * @brief Registers a command and callback to be called when that command is invoked.
     *
     * @param cmd The slashcommand object to register.
     * @param cb The callback function to be called when the slashcommand is invoked.
     */
    void register_slash_command(const dpp::slashcommand& cmd, const event_callback_t& cb);
}

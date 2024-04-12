#pragma once
#include "managedcommand.h"

namespace llpp::discord
{
    void register_command(const ManagedCommand& cmd);

    void clear_command(const ManagedCommand& cmd);

    void clear_commands();

    void slashcommand_callback(const dpp::slashcommand_t& event);

    std::vector<dpp::slashcommand> get_all_commands();

    /**
     * @brief Checks whether a command and the channel it was sent in are valid.
     *
     * @param event The slashcommand event to check whether it is valid.
     *
     * @return True if the command is unauthorized and was handled, false otherwise.
     */
    bool handle_unauthorized_command(const dpp::slashcommand_t& event);
}
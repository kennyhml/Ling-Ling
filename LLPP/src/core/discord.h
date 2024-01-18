#pragma once
#include <dpp/dpp.h>
#include <memory>
#include <string>

namespace llpp::core::discord
{
    using event_callback_t = std::function<void(const dpp::slashcommand_t&)>;

    inline std::unique_ptr<dpp::cluster> bot;

    bool init();

    bool handle_unauthorized_command(const dpp::slashcommand_t& event);
    bool is_user_command_authorized(const dpp::guild_member& user);

    bool is_channel_command_authorized(dpp::snowflake channel);
    bool is_role_command_authorized(dpp::snowflake role);

    void register_slash_command(const dpp::slashcommand& command,
                                const event_callback_t&);

    void inform_started();
    void inform_fatal_error(const std::exception& error, const std::string& task);
}

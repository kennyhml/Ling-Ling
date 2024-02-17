#pragma once
#include <dpp/dpp.h>
#include <memory>
#include <string>
#include <asapp/interfaces/tribemanager.h>

namespace llpp::core::discord
{

    inline std::unique_ptr<dpp::cluster> bot;

    bool init();

    bool handle_unauthorized_command(const dpp::slashcommand_t& event);

    bool is_user_command_authorized(const dpp::guild_member& user);

    bool is_channel_command_authorized(dpp::snowflake channel);

    bool is_role_command_authorized(dpp::snowflake role);



    void inform_started();

    void inform_fatal_error(const std::exception& error, const std::string& task);

    void send_station_disabled(const std::string& station, const std::string& reason);
    void send_station_suspended(const std::string& station, const std::string& reason,
                                std::chrono::minutes duration);

    void handle_tribelogs(const asa::interfaces::TribeManager::LogEntries& all_,
                          const asa::interfaces::TribeManager::LogEntries& new_);
}

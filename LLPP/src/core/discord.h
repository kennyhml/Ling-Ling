#pragma once
#include <chrono>
#include <dpp/dpp.h>
#include <memory>
#include <stdexcept>
#include <string>

namespace llpp::core::discord
{
    using event_callback_t = std::function<void(const dpp::slashcommand_t&)>;

    inline std::unique_ptr<dpp::cluster> bot;

    bool init(const std::string& token);

    void register_slash_command(const dpp::slashcommand& command,
                                const event_callback_t&);

    void inform_started();
    void inform_fatal_error(const std::exception& error, const std::string& task);
}

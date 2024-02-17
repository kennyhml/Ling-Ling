#pragma once
#include <dpp/dpp.h>


namespace llpp::discord
{
    using event_callback_t = std::function<void(const dpp::slashcommand_t&)>;


    inline std::unique_ptr<dpp::cluster> bot;

    bool init();

    void register_slash_command(const dpp::slashcommand& cmd, const event_callback_t& cb);
}

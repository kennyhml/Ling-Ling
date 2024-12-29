#pragma once
#include <dpp/dpp.h>

namespace discord
{
    using slashcommand_and_callback_t = std::pair<dpp::slashcommand,
        std::function<void(const dpp::slashcommand_t&)> >;
}

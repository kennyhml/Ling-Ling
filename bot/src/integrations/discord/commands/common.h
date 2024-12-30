#pragma once
#include <dpp/dpp.h>

namespace discord
{
    // Function type to handle a slashcommand event
    using command_callback_t = std::function<void(dpp::cluster*,
                                                  const dpp::slashcommand_t&)>;

    // Slashcommand and the callback to handle the command being triggered
    using command_register_t = std::pair<dpp::slashcommand, command_callback_t>;

    // Callback function to attach a subcommand with a callback to handle it being triggered
    using command_create_t = std::function<command_callback_t(dpp::slashcommand&)>;

    // Alias for dpp confirmation callback lambda input
    using conf_t = const dpp::confirmation_callback_t&;

    /**
     * @brief Helper function to try get a variant from a command value.
     */
    template<typename T>
    [[nodiscard]] T tget(const dpp::command_value& value)
    {
        try {
            return std::get<T>(value);
        } catch (const std::bad_variant_access&) {
            return T();
        }
    }
}

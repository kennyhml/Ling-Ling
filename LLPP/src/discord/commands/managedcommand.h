#pragma once
#include <dpp/dpp.h>

namespace llpp::discord
{
    using event_callback_t = std::function<void(const dpp::slashcommand_t&, void* data)>;

    class ManagedCommand final : public dpp::slashcommand
    {
    public:
        ManagedCommand(const std::string& t_name, const std::string& t_description,
                       dpp::snowflake t_application_id,
                       event_callback_t t_callback,
                       bool t_is_static = false,
                       void* t_data = nullptr
        );

        /**
         * @brief Checks if this command should be deleted when the bot re-initializes.\n
         * A static command is command that will stay the same from its first creation
         * onwards.
         *
         * Example of a static command: Crate rerolling command.\n
         * Example of a nonstatic command: Crafting Station disable/enable.
         *
         * @returns True if the command is static, false otherwise.
         *
         */
        [[nodiscard]] bool is_static() const { return is_static_; }

        /**
         * @brief Gets the callback to handle this slashcommand.
         */
        [[nodiscard]] const event_callback_t& get_callback() const { return callback_; }

        [[nodiscard]] void* get_extra_data() const { return extra_data_; }


    private:
        void* extra_data_;

        bool is_static_;
        event_callback_t callback_;
    };
}

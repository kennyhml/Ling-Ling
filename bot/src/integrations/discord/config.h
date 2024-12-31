#pragma once
#include <snowflake.h>

#include "configuration/common.h"
#include "configuration/managedvar.h"
#include "configuration/validate.h"
#include "core/startup.h"

template<>
struct lingling::json_traits<dpp::snowflake>
{
    using to_json_t = std::function<json_t(const dpp::snowflake&)>;
    using from_json_t = std::function<dpp::snowflake(const json_t&)>;

    to_json_t to_json = [](const dpp::snowflake& value) {
        return json_t(value.str());
    };

    from_json_t from_json = [](const json_t& json) {
        return dpp::snowflake(json.get<std::string>());
    };
};

namespace lingling::discord
{
    /**
     * @brief Validates the discord configuration for the values needed required.
     *
     * @param json The data to validate, if empty it will be populated with defaults.
     *
     * @return Whether changes were made to the data.
     */
    bool validate_discord_config(json_t& json);

    /**
     * @brief Gets a reference to the json data that the base discord config resides in.
     */
    json_t& get_base_config();

    /**
     * @brief Gets a reference to the json data that the channel discord config resides in.
     */
    json_t& get_channel_config();

    // Basic configuration
    inline managed_var<std::string> bot_token{"token", get_base_config};
    inline managed_var<dpp::snowflake> guild_id{"guild", get_base_config};

    // Channel configuration
    inline managed_var<dpp::snowflake> category{"category", get_channel_config};
    inline managed_var<dpp::snowflake> comands{"commands", get_channel_config};
    inline managed_var<dpp::snowflake> dashboard{"dashboard", get_channel_config};

    // Attach the config validation strategy on startup
    inline run_on_startup startup_register_validation([] {
        add_config_validation("discord", validate_discord_config);
    });
}

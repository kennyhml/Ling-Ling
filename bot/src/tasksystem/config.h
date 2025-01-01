#pragma once
#include "configuration/common.h"
#include "configuration/managedvar.h"
#include <dpp/dpp.h>

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

namespace lingling
{
    /**
     * @brief Validates the discord configuration for the values needed required.
     *
     * @param json The data to validate, if empty it will be populated with defaults.
     *
     * @return Whether changes were made to the data.
     */
    bool validate_tasksystem_config(json_t& json);

    /**
     * @brief Gets a reference to the json data that the base discord config resides in.
     */
    json_t& get_tasksystem_config();

    // Basic configuration
    inline managed_var<dpp::snowflake> q_channel{"q_channel", get_tasksystem_config};
}

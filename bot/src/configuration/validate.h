#pragma once
#include <functional>
#include <string>
#include "common.h"

namespace lingling
{
    // Function to call to validate the provided json data, returns whether the data was
    // modified or not as an indicator whether the data should be dumped.
    using validation_callback_t = std::function<bool(json_t&)>;

    /**
     * @brief Register a callback to validate a certain category in the settings.
     *
     * @param category The name of the category to add the validator for, e.g "gacha-bot"
     * @param fn The function to call to validate the configuration of that category.
     */
    void add_config_validation(const std::string& category, validation_callback_t fn);

    /**
     * @brief Recursively validates a json object against a given schema.
     * - Any key thats not in the schema will be erased from the object to validate.
     * - Any key from the schema not in the object will be added as default value.
     * - Any sub json object in the given object is validated recursively.
     *
     * @param name The name of the "file" we are validating for logging purposees.
     * @param to_validate The json object to validate and patch.
     * @param schema The schema to validate the json object with.
     *
     * @return True if any patches were made to the data, false otherwise.
     */
    bool patch_json(const std::string& name, json_t& to_validate, const json_t& schema);

    /**
     * @brief Validates that the required configuration files exist and the settings within
     * them match their respective schema. Modules may register their own validation callbacks
     * to perform validation on their config through @link add_config_validation \endlink.
     */
    void validate_config_integrity();
}

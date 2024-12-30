#include "validate.h"
#include "utility.h"
#include "exceptions.h"

#include <set>
#include <asa/core/logging.h>

namespace lingling
{
    namespace
    {
        std::map<std::string, validation_callback_t> validation_callbacks;
    }

    void add_config_validation(const std::string& category, validation_callback_t fn)
    {
        // Make sure there isnt already callback for the category.
        if (validation_callbacks.contains(category)) {
            throw duplicate_category_callback(category);
        }
        validation_callbacks.emplace(category, std::move(fn));
    }

    void validate_config_integrity()
    {
        const auto path = get_config_path();
        asa::get_logger()->info("Performing config validation: {}..", path.string());

        if (!exists(path)) {
            asa::get_logger()->warn("Configuration file does not exist, creating..");

            // Only create up to the parent, the last component is the file, not a directory.
            create_directories(path.parent_path());
            utility::dump(json_t(), get_config_path());
        }

        json_t& data = load_config_data();
        bool patches = false;
        std::set<std::string> called;
        for (const auto& [k, v]: data.items()) {
            if (!validation_callbacks.contains(v)) {
                asa::get_logger()->warn("No validation is registered for '{}'!", k);
                continue;
            }
            // Call the validation function registered for this category and check if
            // it has made any changes so that we need to dump.
            patches |= validation_callbacks.at(k)(v);
            called.emplace(k);
        }

        // Its possible a category didnt exist in the file and needs to be created now.
        // In that case it hasnt been called in the previous validation
        for (const auto& [k, v]: validation_callbacks) {
            if (!called.contains(k)) {
                // This category was not validated because it didnt exist
                // The validation function will receive an empty json object reference to
                // populate with the default data.
                data[k] = json_t();
                patches |= v(data.at(k));
            }
        }

        // If any changes were made to the data during the validation process, dump them
        // so we dont have to perform them again on next startup (duh).
        if (patches) { utility::dump(data, get_config_path()); }
    }
}

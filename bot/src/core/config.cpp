#include "config.h"

#include <iostream>

#include "asa/core/logging.h"
#include "configuration/validate.h"
#include "utility/utility.h"

namespace lingling
{
    namespace
    {
        const json_t CORE_CONFIG_SCHEMA = R"(
        {
            "user_name": "",
            "asa_directory": ""
        })"_json;

        const auto USERNAME_SETUP_MSG =
                "\n======================================================================================\n"
                "\033[1;31mNo user name was found in the configuration data!\033[0m\n"
                "The name is required to properly identifiy the application host of the bot.\n\n"
                "Please enter a user name that:\n"
                "   - Clearly let's your tribemates understand who is running the bot.\n"
                "   - Uniquely identifies the bot machine (if you are running multiple)."
                "\n======================================================================================\n";
    }

    bool validate_core_config(json_t& json)
    {
        return patch_json("Core Config", json, CORE_CONFIG_SCHEMA);
    }

    json_t& get_core_config()
    {
        return get_config_data().at("core");
    }

    void ensure_user_name_provided()
    {
        if (!user_name.get().empty()) {
            asa::get_logger()->info("Known user name: '{}'.", user_name.get());
            return;
        }

        utility::enable_virtual_terminal_processing();
        std::cout << USERNAME_SETUP_MSG;

        std::string name;
        do {
            std::cout << "\033[1;36mUser name\033[0m: ";
            std::getline(std::cin, name);
        } while (name.empty());
        user_name.set(name);
        asa::get_logger()->info("User name set to: '{}'.", user_name.get());
    }
}

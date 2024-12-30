#pragma once
#include <filesystem>
#include <nlohmann/json.hpp>

namespace lingling
{
    using json_t = nlohmann::ordered_json;

    /**
     * @brief Gets the configuration path, all the configuration files are to be stored
     * at C:\ProgramData\Ling-Ling\config.json
     *
     * @return The path to the desired configuration.
     */
    [[nodiscard]] std::filesystem::path get_config_path();

    /**
     * @brief Serves as a static storage for the data loaded from the config file.
     *
     * @return A static reference to the data loaded from the json file.
     */
    [[nodiscard]] json_t& get_config_data();
}

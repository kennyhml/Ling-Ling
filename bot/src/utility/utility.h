#pragma once
#include <functional>
#include <nlohmann/json.hpp>

namespace lingling::utility
{
    /**
     * @brief Struct to run a piece of code when the static initialization phase begins.
     *
     * This could, for example, be used to register callbacks from a module without having
     * to explicitly call / know that module.
     */
    struct run_on_initialize final
    {
        explicit run_on_initialize(const std::function<void()>& fn) { fn(); }
    };

    /**
     * @brief Writes the given json data to the given file.
     *
     * @param file The path to the file to write the data to.
     * @param data The data to dump into the file, note that all other content is erased.
     */
    void dump(const nlohmann::ordered_json& data, const std::filesystem::path& file);

    /**
     * @brief Reads the json data from a given file path.
     *
     * @param file The path to the file to parse the json data from.
     *
     * @return The ordered json data of the file.
     */
    [[nodiscard]] nlohmann::ordered_json read(const std::filesystem::path& file);

    void enable_virtual_terminal_processing();
}


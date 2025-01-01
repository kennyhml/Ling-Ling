#pragma once
#include <message.h>
#include <nlohmann/json.hpp>

namespace lingling::utility
{
    std::vector<dpp::message> extract_messages_sorted(const dpp::message_map&);

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

    std::string make_counter(std::chrono::system_clock::duration duration);
}

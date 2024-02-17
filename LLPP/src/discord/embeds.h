#pragma once
#include <string>
#include <dpp/dpp.h>

namespace llpp::discord
{
    /**
     * @brief Creates an embed that displays information about ling ling starting.
     */
    dpp::embed get_started_embed();

    /**
     * @brief Creates an embed that displays information about a fatal error that has
     * crashed ling ling.
     */
    dpp::embed get_fatal_error_embed(const std::exception& error,
                                     const std::string& task);

    /**
     * @brief Creates an embed that displays information about a station having been
     * disabled automatically.
     */
    dpp::embed get_station_disabled_embed(const std::string& station,
                                          const std::string& reason);

    /**
     * @brief Creates an embed that displays information about a stations completion having
     * been suspended
     */
    dpp::embed get_station_suspended_embed(const std::string& station,
                                           const std::string& reason,
                                           std::chrono::minutes duration);
}

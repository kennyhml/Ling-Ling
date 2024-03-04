#pragma once
#include <string>
#include <asapp/structures/cavelootcrate.h>
#include <dpp/dpp.h>

namespace llpp::discord
{
    /**
     * @brief Creates a message that displays information about ling ling starting.
     */
    dpp::message create_started_message();

    /**
     * @brief Creates a message that displays information about a fatal error that has
     * crashed ling ling.
     */
    dpp::message create_fatal_error_message(const std::exception& error,
                                            const std::string& task);

    /**
    * @brief Creates a message to display information about an error that occured
    * but was handled accordingly.
    */
    dpp::message create_error_message(const std::exception& error, bool ping = true);

    /**
     * @brief Creates a message that displays information about a station having been
     * disabled automatically.
     */
    dpp::message create_station_disabled_message(const std::string& station,
                                                 const std::string& reason);

    /**
     * @brief Creates a message that displays information about a stations completion
     having been suspended
     */
    dpp::message create_station_suspended_message(const std::string& station,
                                                  const std::string& reason,
                                                  std::chrono::minutes duration);
}

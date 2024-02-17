#pragma once
#include <asapp/interfaces/tribemanager.h>

namespace llpp::discord
{
    /**
     * @brief Handles the discord tribelog components by updating the tribelog stream
     * and sending alerts (if any alarming new events are found).
     *
     * @param all_events A vector containing every recent log event.
     * @param new_events A vector containing only new log events.
     */
    void handle_tribelog_events(
        const asa::interfaces::TribeManager::LogEntries& all_events,
        const asa::interfaces::TribeManager::LogEntries& new_events);
}

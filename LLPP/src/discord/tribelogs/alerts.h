#pragma once
#include <asapp/interfaces/tribemanager.h>

namespace llpp::discord
{
    /**
     * @brief Handle sending any sort of alert events contained in the new entries.
     *
     * @param events The new entries that were found within a log update.
     */
    void handle_alerts(const asa::interfaces::TribeManager::LogEntries& events);
}

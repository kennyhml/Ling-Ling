#pragma once
#include <asapp/interfaces/tribemanager.h>

namespace llpp::discord
{
    /**
     * @brief Handles sending the logs to the log overview channel.
     *
     * @param entries The entries to update the channel with.
     * @param flush_previous_messages If true, the previous messages are overwritten.
     * Otherwise the messages are concatenated.
     */
    void handle_stream(const asa::interfaces::TribeManager::LogEntries& entries,
                       bool flush_previous_messages = false);
}

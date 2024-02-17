#include "handler.h"
#include "alerts.h"
#include "stream.h"
#include "../../config/config.h"

namespace llpp::discord
{
    void handle_tribelog_events(
        const asa::interfaces::TribeManager::LogEntries& all_events,
        const asa::interfaces::TribeManager::LogEntries& new_events)
    {
        handle_alerts(new_events);
        // Handle the correct set of events depending on the configuration
        // for the log post type
        if (config::discord::advanced::flush_logs.get()) {
            handle_stream(all_events, false);
        }
        else { handle_stream(new_events, true); }
    }
}

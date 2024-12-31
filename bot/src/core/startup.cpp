#include "startup.h"
#include "configuration/validate.h"
#include "integrations/discord/discord.h"

#include <asa/core/logging.h>

namespace lingling
{
    namespace
    {
        // Array of functions to be called at startup, implement as getter to prevent
        // issues with initialization order
        std::vector<std::function<void()> >& get_deferred(const bool early)
        {
            static std::vector<std::function<void()> > deferred_early;
            static std::vector<std::function<void()> > deferred_late;
            return early ? deferred_early : deferred_late;
        }
    }

    void defer_to_startup(const std::function<void()>& fn, const bool early_startup)
    {
        get_deferred(early_startup).push_back(fn);
    }

    void startup()
    {
        asa::get_logger()->info("Performing early startup..");
        for (const auto& fn: get_deferred(true)) { fn(); }

        asa::get_logger()->info("Performing core startup..");
        validate_config_integrity();

        for (const auto& fn: get_deferred(false)) { fn(); }
        asa::get_logger()->info("Startup completed.");
    }
}

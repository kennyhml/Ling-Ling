#include "startup.h"
#include "configuration/validate.h"

#include <asa/core/logging.h>

namespace lingling
{
    namespace
    {
        // Array of functions to be called at startup, implement as getter to prevent
        // issues with initialization order
        std::vector<std::function<void()> >& get_deferred(const startup_time when)
        {
            static std::vector<std::function<void()> > deferred_early;
            static std::vector<std::function<void()> > deferred_late;
            return when == startup_time::STARTUP_EARLY ? deferred_early : deferred_late;
        }
    }

    void defer_to_startup(const std::function<void()>& fn, const startup_time time)
    {
        get_deferred(time).push_back(fn);
    }

    void startup()
    {
        asa::get_logger()->info("Performing startup phase 1/3..");
        for (const auto& fn: get_deferred(startup_time::STARTUP_EARLY)) { fn(); }

        asa::get_logger()->info("Performing startup phase 1/2..");
        validate_config_integrity();

        asa::get_logger()->info("Performing startup phase 3/3..");
        for (const auto& fn: get_deferred(startup_time::STARTUP_LATE)) { fn(); }
        asa::get_logger()->info("Startup completed.");
    }
}

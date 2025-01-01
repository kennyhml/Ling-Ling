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

        std::chrono::system_clock::time_point startup_timestamp;
    }

    void defer_to_startup(const startup_time time, const std::function<void()>& fn)
    {
        get_deferred(time).push_back(fn);
    }

    void startup()
    {
        startup_timestamp = std::chrono::system_clock::now();
        asa::get_logger()->info("Performing startup phase 1/3..");
        const auto& early_deferred = get_deferred(startup_time::STARTUP_EARLY);
        for (size_t i = 0; i < early_deferred.size(); ++i) {
            early_deferred[i]();
        }

        asa::get_logger()->info("Performing startup phase 1/2..");
        validate_config_integrity();

        asa::get_logger()->info("Performing startup phase 3/3..");
        const auto& late_deferred = get_deferred(startup_time::STARTUP_LATE);
        for (size_t i = 0; i < late_deferred.size(); ++i) {
            late_deferred[i]();
        }
        asa::get_logger()->info("Startup completed.");
    }

    std::chrono::system_clock::time_point get_startup_time()
    {
        return startup_timestamp;
    }
}

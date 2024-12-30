#include "startup.h"

#include "asa/core/logging.h"
#include "configuration/validate.h"
#include "integrations/discord/discord.h"

namespace lingling
{
    namespace
    {
        // Array of functions to be called at startup, implement as getter to prevent
        // issues with initialization order
        std::vector<std::function<void()>>& get_deferred()
        {
            static std::vector<std::function<void()>> deferred;
            return deferred;
        }
    }

    void defer_to_startup(const std::function<void()>& fn)
    {
        get_deferred().push_back(fn);
    }

    void startup()
    {
        asa::get_logger()->info("Performing startup..");
        for (const auto& fn: get_deferred()) { fn(); }

        validate_config_integrity();
        discord::startup();
    }
}

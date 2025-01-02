#include "state.h"

#include <asa/core/logging.h>
#include <magic_enum.hpp>

namespace lingling
{
    namespace
    {
        auto application_state = app_state::APP_STATE_ONLINE;
    }

    void set_application_state(const app_state state)
    {
        const auto old = application_state;
        application_state = state;
        asa::get_logger()->info("Application state changed from '{}' to '{}'.",
                                magic_enum::enum_name(state), magic_enum::enum_name(old));
    }

    app_state get_application_state()
    {
        return application_state;
    }
}

#pragma once
#include "core/startup.h"

namespace lingling::gacha
{
    void initialize_module();

    inline run_on_startup startup{startup_time::STARTUP_EARLY, initialize_module};
}

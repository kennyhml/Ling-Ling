#pragma once
#include <cstdint>

namespace llpp::bots::boss
{
    enum CrateTier : int32_t
    {
        GAMMA,
        BETA,
        ALPHA
    };

    /**
     * @brief Collects the element crate given the tier to collect
     *
     * @param tier The tier of the boss that dropped the crate, used for waypoint color.
     * @return True if the crate was collected before the timer ran out, false otherwise.
     */
    bool collect_element_crate(CrateTier tier);
}

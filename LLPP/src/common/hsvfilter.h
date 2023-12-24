#pragma once

namespace llpp::util
{
    struct HsvFilter
    {
        int hue_min{0};
        int hue_max{179};

        int sat_min{0};
        int sat_max{255};
        int sat_add{0};
        int sat_sub{0};

        int val_min{0};
        int val_max{255};
        int val_add{0};
        int val_sub{0};
    };
}

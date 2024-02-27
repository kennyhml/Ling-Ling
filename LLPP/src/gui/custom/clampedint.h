#pragma once


namespace llpp::gui::custom
{
    bool ClampedInputInt(const char* label, int* v, int min, int max, int step = 1,
                         int step_fast = 5);
}

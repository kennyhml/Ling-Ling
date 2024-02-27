#include "clampedint.h"

#include <algorithm>

#include "../../../external/imgui/imgui.h"


namespace llpp::gui::custom
{
    bool ClampedInputInt(const char* label, int* v, const int min, const int max,
                         const int step, const int step_fast)
    {
        const bool result = ImGui::InputInt(label, v, step, step_fast);
        if (result) { *v = std::ranges::clamp(*v, min, max); }
        return result;
    }
}

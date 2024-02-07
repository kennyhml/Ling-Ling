#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS

#include "../../external/imgui/imgui.h"

namespace llpp::gui
{
    ImColor get_accent_color(float a = 1.f);


    void begin_child(const char* name, ImVec2 size);
    void end_child();
}

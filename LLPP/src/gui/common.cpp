#include "common.h"
#include "../../external/imgui/imgui_internal.h"
#include "../../external/imgui/imgui_stdlib.h"
#include "state.h"

namespace llpp::gui
{
    namespace
    {
        int accent_color[4] = {140, 131, 214, 255};
    }

    ImColor get_accent_color(float a)
    {
        return {
            accent_color[0] / 255.f, accent_color[1] / 255.f, accent_color[2] / 255.f, a
        };
    }

    void begin_child(const char* name, ImVec2 size)
    {
        const ImGuiWindow* window = ImGui::GetCurrentWindow();
        const ImVec2 pos = window->DC.CursorPos;

        ImGui::BeginChild(std::string(name).append(".main").c_str(), size, false,
                          ImGuiWindowFlags_NoScrollbar);

        // draw the name of this tab
        ImGui::GetWindowDrawList()->AddRectFilled(pos, pos + size,
                                                  ImColor(28 / 255.f, 30 / 255.f,
                                                          36 / 255.f, state::animation),
                                                  4);
        ImGui::GetWindowDrawList()->AddText(pos + ImVec2(10, 5),
                                            get_accent_color(state::animation), name,
                                            ImGui::FindRenderedTextEnd(name));

        ImGui::SetCursorPosY(30);
        ImGui::BeginChild(name, {size.x, size.y - 30});
        ImGui::SetCursorPosX(10);

        ImGui::BeginGroup();

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, state::animation);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {8, 8});
    }

    void end_child()
    {
        ImGui::PopStyleVar(2);
        ImGui::EndGroup();
        ImGui::EndChild();
        ImGui::EndChild();
    }
}

#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include <string>
#include <functional>
#include <string>

#include "state.h"
#include "../../external/imgui/imgui.h"

namespace llpp::gui
{




    bool tab_button(const char* icon, const char* label, bool selected, float rounding,
                    ImDrawFlags flags);
    bool sub_tab_button(const char* label, bool selected);
    void render_main_tab_area(const std::string& name, ImVec2 size,
                              const std::function<void()>& render_content);

    template <typename T>
    void render_subtab_buttons(const std::vector<const char*>& buttons, T& selected)
    {
        ImGui::SetCursorPos(ImVec2(state::maintabs_data.width + 15, 33));
        ImGui::BeginGroup();
        for (int i = 0; i < buttons.size(); i++) {
            const bool is_selected = (selected == i);
            if (sub_tab_button(buttons[i], is_selected) && !is_selected) {
                selected = static_cast<T>(i);
                state::animation = 0.f;
            }
            if (i != buttons.size() - 1) { ImGui::SameLine(); }
        }
        ImGui::EndGroup();
    }

    void draw_general_ark_tabs();
    void draw_general_bot_tabs();


    void draw_discord_bot_config();
    void draw_discord_info_tabs();
    void draw_discord_alert_tabs();

    void draw_bots_drops_tab();
    void draw_bots_crops_tabs();
    void draw_bots_sap_tabs();
    void draw_bots_crafting_tabs();
    void draw_bots_phoenix_tabs();
}

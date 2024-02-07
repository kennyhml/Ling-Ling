#include "dialogs.h"
#include "../../../external/imgui/imgui_internal.h"


namespace llpp::gui::custom
{
    namespace
    {
        char element_buffer[256] = {};
    }

    bool simple_add_dialog(bool* active, const char* text, std::string& new_out)
    {
        ImGui::OpenPopup(text);
        if (!ImGui::BeginPopupModal(text, active, ImGuiWindowFlags_AlwaysAutoResize)) {
            // Popup is not yet open.
            return false;
        }

        // Clear the allocated buffer while the active is being created.
        if (ImGui::IsWindowAppearing()) {
            memset(element_buffer, 0, sizeof(element_buffer));
        }

        ImGui::InputText("##simple_add_dialog", element_buffer,
                         IM_ARRAYSIZE(element_buffer));

        bool done = false;
        if (ImGui::Button("OK##simple_add_dialog")) {
            done = true;
            *active = false;
            new_out = element_buffer;
        }
        ImGui::EndPopup();
        return done;
    }

    bool simple_delete_dialog(bool* active, const char* text, bool& confirmed)
    {
        ImGui::OpenPopup(text);
        if (!ImGui::BeginPopupModal(text, active, ImGuiWindowFlags_AlwaysAutoResize)) {
            return false;
        }
        ImGui::Text(
            "Are you absolutely sure you want to do this?\nThis action is irreversible!");

        const bool ok_clicked = ImGui::Button("OK##simple_delete_dialog", ImVec2(80, 0));
        ImGui::SameLine();
        const bool cancel_clicked = ImGui::Button("Cancel##simple_delete_dialog");

        if (ok_clicked || cancel_clicked) {
            *active = false;
            confirmed = ok_clicked;
        }

        ImGui::EndPopup();
        return ok_clicked || cancel_clicked;
    }
}

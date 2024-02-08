#include "combo_dynamic.h"
#include "../../../external/imgui/imgui.h"
#include <format>
#include "dialogs.h"
#include <string>

namespace llpp::gui::custom
{
    namespace
    {
        std::string delete_text;
        std::string rename_text;
    }

    bool Combo(const char* label, int* selected, std::vector<const char*>& items,
               const CustomComboFlags_ flags, const char* tooltip, const char* add_label,
               const char* del_label, const char* rename_label,
               const char* add_popup_text, bool* add_popup, bool* remove_popup,
               bool* rename_popup, const std::function<void(const char* new_)>& on_added,
               const std::function<void(const char* deleted_)>& on_delete,
               const std::function<void(const char* old, const char* new_)>& on_rename,
               const int popup_max_height_in_items)
    {
        bool ret = ImGui::Combo(label, selected, items.data(), items.size(),
                                popup_max_height_in_items);

        if (strlen(tooltip) && ImGui::IsItemHovered()) { ImGui::SetTooltip(tooltip); }

        // Buttons go below if the flag is set, otherwise to the right.
        if (!(flags & CustomComboFlags_ButtonsBelow)) { ImGui::SameLine(); };
        if (flags & CustomComboFlags_AllowAdd) {
            if (ImGui::Button(add_label)) { *add_popup = true; }
        }

        if (flags & CustomComboFlags_AllowRemove) {
            ImGui::SameLine();
            if (ImGui::Button(del_label) && !items.empty()) {
                if (flags & CustomComboFlags_ConfirmRemove) { *remove_popup = true; }
                else {
                    // Erase the element based on its index in the vector, make sure we
                    // also fix the current index after so it doesnt invalidate.
                    on_delete(items[*selected]);
                    items.erase(items.begin() + *selected);
                    *selected = std::max(0, *selected - 1);
                    ret = true;
                }
            }
        }

        if (flags & CustomComboFlags_AllowRename) {
            ImGui::SameLine();
            if (ImGui::Button(rename_label) && !items.empty()) { *rename_popup = true; }
        }

        if (*add_popup) {
            std::string new_;
            if (simple_add_dialog(add_popup, add_popup_text, new_)) {
                items.push_back(_strdup(new_.c_str()));
                *selected = items.size() - 1;
                on_added(items[*selected]);
                ret = true;
            }
        }

        if (*remove_popup) {
            bool del = false;
            delete_text = std::format("Confirm to delete '{}'", items[*selected]);
            if (simple_delete_dialog(remove_popup, delete_text.c_str(), del) && del) {
                on_delete(items[*selected]);
                items.erase(items.begin() + *selected);
                *selected = std::max(0, *selected - 1);
                ret = true;
            }
        }

        if (*rename_popup) {
            std::string new_;
            bool confirmed = false;
            rename_text = std::format("Give '{}' a new name!", items[*selected]);
            if (simple_rename_dialog(rename_popup, rename_text.c_str(), confirmed, new_)
                && confirmed) {
                on_rename(items[*selected], new_.c_str());
                items[*selected] = _strdup(new_.c_str());
                ret = true;
            }
        }


        return ret;
    }
}

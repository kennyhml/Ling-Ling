#pragma once
#include <functional>
#include <vector>

enum CustomComboFlags_ : int
{
    CustomComboFlags_AllowAdd = 1,
    CustomComboFlags_AllowRemove = 1 << 1,
    CustomComboFlags_AllowRename = 1 << 2,
    CustomComboFlags_ConfirmRemove = 1 << 3,

    CustomComboFlags_Default = CustomComboFlags_AllowAdd | CustomComboFlags_AllowRemove,
};

namespace llpp::gui::custom
{
    bool Combo(const char* label, int* selected, std::vector<const char*>& items,
               CustomComboFlags_ flags = CustomComboFlags_Default,
               const char* tooltip = nullptr, const char* add_label = nullptr,
               const char* del_label = nullptr, const char* rename_label = nullptr,
               const char* add_popup_text = nullptr, bool* add_popup = nullptr,
               bool* remove_popup = nullptr, bool* rename_popup = nullptr,
               const std::function<void(const char* new_)>& on_added = nullptr,
               const std::function<void(const char* deleted_)>& on_delete = nullptr,
               int popup_max_height_in_items = -1);
}

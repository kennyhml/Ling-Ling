#include "parasaur.h"
#include "common.h"
#include "state.h"
#include "../config/config.h"
#include "custom/combo_dynamic.h"

namespace llpp::gui
{
    namespace
    {
        int selected = 0;

        bool add_dialog = false;
        bool del_dialog = false;
        bool rename_dialog = false;

        void add_callback(const char* added)
        {
            std::cout << added << " added!" << std::endl;
        }

        void del_callback(const char* deleted)
        {
            std::cout << deleted << " deleted!" << std::endl;
        }
    }

    void draw_bots_parasaur_tab()
    {
        begin_child("Parasaur Stations",
                    ImVec2(425 - state::maintabs_data.width,
                           ImGui::GetWindowHeight() * 0.33f));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Station:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({100, 11});
            std::vector<const char*>* data = config::bots::parasaur::stations.get_ptr();

            auto flags = CustomComboFlags_Default | CustomComboFlags_AllowRename |
                CustomComboFlags_ConfirmRemove;

            custom::Combo("##selected_manager", &selected, *data,
                          static_cast<CustomComboFlags_>(flags), "Tooltip", "+##parasaur",
                          "-##parasaur", "r##parasaur", "Add parasaur", &add_dialog,
                          &del_dialog, &rename_dialog, add_callback, del_callback);
        }
        end_child();
    }
}

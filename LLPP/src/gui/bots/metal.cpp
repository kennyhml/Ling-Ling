#include "metal.h"
#include "../common.h"
#include "../state.h"
#include "../../config/config.h"
#include "../custom/combo_dynamic.h"
#include "../../../external/imgui/imgui_stdlib.h"
#include "../../auth/auth.h"
#include "../custom/clampedint.h"

namespace llpp::gui
{
    using namespace config::bots::farm;

    namespace
    {
        const std::vector<std::string> authorized{
            "cole", "Sleepyv9000", "Sleepv4", "Sleepyv3", "Sleepyv2", "Sleepy", "Swav",
            "Justin", "Dev", "lingling", "Shorty"
        };

        int selected_manager = 0;

        bool add_dialog = false;
        bool del_dialog = false;
        bool rename_dialog = false;

        auto const MANAGER_TOOLTIP =
                "Create, remove or rename your Farm Managers here.\n"
                "A Farm Manager handles the completion of a group of farm teleporters.";

        bots::metal::MetalManagerConfig get_default_config()
        {
            bots::metal::MetalManagerConfig _default;
            _default.prefix = "";
            _default.last_completed = 0;
            _default.num_stations = 1;
            _default.interval = 120;
            _default.on_changed = config::save;

            return _default;
        }

        /**
         * @brief Callback to handle a new manager being added.
         * Creates the default configuration for it and syncs it with the JSON.
         *
         * @param added The name of the new manager that was added.
         */
        void on_added(const char* added)
        {
            auto& _new = configs[added] = config::ManagedVar({"bots", "farm", added},
                             config::save, get_default_config());
            _new.save();
        }

        /**
         * @brief Callback to handle a manager being deleted.
         * Removes its configuration from the internal data and syncs it with the JSON.
         *
         * @param deleted The name of the manager that was deleted.
         */
        void on_deleted(const char* deleted)
        {
            configs[deleted].erase(); // Removes it from the JSON
            configs.erase(deleted);
        }

        /**
         * @brief Callback to handle a parasaur station being renamed.
         * Changes the internal name and syncs it with the JSON.
         *
         * @param old_name The previous name of the station that was renamed.
         * @param new_name The new name of the station that was renamed.
         */
        void on_renamed(const char* old_name, const char* new_name)
        {
            // Easiest way to rename is to just create a copy and delete the old one.
            auto new_ = configs[old_name];
            new_.set_path({"bots", "farm", new_name});
            auto& ref = configs[new_name] = new_;
            ref.save();

            // Now remove the old object
            configs[old_name].erase();
            configs.erase(old_name);
        }

        config::ManagedVar<bots::metal::MetalManagerConfig>* get_active_manager()
        {
            // No stations exist at all.
            if (managers.get_ref().empty()) { return nullptr; }
            const std::string name = managers.get_ref()[selected_manager];

            // Make sure config for this station exists, if not just create a default.
            if (!configs.contains(name)) {
                configs[name] = config::ManagedVar({"bots", "farm", name}, config::save,
                                                   get_default_config());
            };
            return &configs[name];
        }
    }

    void draw_metal_tab()
    {
        if (!std::ranges::contains(authorized, auth::user)) { return; }

        const ImVec2 managers_dimensions(425 - state::maintabs_data.width,
                                         ImGui::GetWindowHeight() * 0.33f);
        begin_child("Farm Managers", managers_dimensions); {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Manager:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.7f);
            ImGui::SetCursorPos({80, 11});
            std::vector<const char*>* data = managers.get_ptr();

            auto flags = CustomComboFlags_Default | CustomComboFlags_AllowRename |
                         CustomComboFlags_ConfirmRemove | CustomComboFlags_ButtonsBelow;

            const bool changed = custom::Combo("##selected_manager", &selected_manager,
                                               *data, static_cast<CustomComboFlags_>(
                                                   flags), MANAGER_TOOLTIP,
                                               "Add New##metal",
                                               "Delete Current##metal",
                                               "Rename Current##metal",
                                               "Add Farm Manager",
                                               &add_dialog, &del_dialog, &rename_dialog,
                                               on_added, on_deleted, on_renamed);
            // Make sure we sync our manager in the config by saving any changes made.
            if (changed) { managers.save(); }
        }
        end_child();
        ImGui::SameLine();
        const ImVec2 advanced_dimensions(270, ImGui::GetWindowHeight() * 0.6f);
        begin_child("General Configuration", advanced_dimensions); {
            if (ImGui::Checkbox("All Disabled", disabled.get_ptr())) { disabled.save(); }
        }
        end_child();

        if (const auto active = get_active_manager(); active != nullptr) {
            ImGui::SetCursorPosY((ImGui::GetWindowHeight() * 0.33f) + 5);
            const ImVec2 configuration_dimensions(managers_dimensions.x,
                                                  ImGui::GetWindowHeight() * 0.66f);
            begin_child("Configuration", configuration_dimensions); {
                ImGui::SetCursorPos({10, 14});
                ImGui::Text("Prefix:");
                ImGui::SetCursorPos({130, 11});
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
                if (ImGui::InputText("##farm_prefix", &active->get_ptr()->prefix)) {
                    active->save();
                }

                ImGui::SetCursorPos({10, 45});
                ImGui::Text("Stations:");
                ImGui::SetCursorPos({130, 42});
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
                if (custom::ClampedInputInt("##farm_stations",
                                            &active->get_ptr()->num_stations, 1, 200)) {
                    active->save();
                }

                ImGui::SetCursorPos({10, 76});
                ImGui::Text("Interval (min):");
                ImGui::SetCursorPos({130, 73});
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
                if (custom::ClampedInputInt("##farm_interval",
                                            &active->get_ptr()->interval, 30, 300)) {
                    active->save();
                }

                if (ImGui::Checkbox("Disabled", &active->get_ptr()->disabled)) {
                    active->save();
                }
                ImGui::SameLine();
                if (ImGui::Checkbox("Logcheck", &active->get_ptr()->check_logs)) {
                    active->save();
                }
            }
            end_child();
        }
    }
}

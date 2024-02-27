#include "parasaur.h"
#include "../common.h"
#include "../state.h"
#include "../../config/config.h"
#include "../custom/combo_dynamic.h"
#include "../../../external/imgui/imgui_stdlib.h"

namespace llpp::gui
{
    using namespace config::bots::parasaur;

    namespace
    {
        int selected_station = 0;

        bool add_dialog = false;
        bool del_dialog = false;
        bool rename_dialog = false;

        bool has_loaded_mode = false;
        int selected_mode = 0;
        const std::vector MODES{"INTERVAL", "MIN READY"};
        const std::vector ALERT_MODES{"@ALERT ROLE", "@HERE", "@EVERYONE", "SILENCED"};

        const char* STATION_TOOLTIP =
            "Create, remove or rename your parasaur stations here.\n"
            "To change the properties of a station, select the station and change the config.";

        const char* MODE_TOOLTIP =
            "Choose the mode that determines when the parasaur bot is initiated.\n\n"
            "INTERVAL: Runs when the given interval expires.\n"
            "MIN READY: Runs when the minimum amount of stations are ready.\n\n"
            "In any case, only the stations whose interval has expired are completed.";


        const char* START_BED_TOOLTIP =
            "The full name of the start bed to spawn on for teleport parasaur stations.\n"
            "This station must have the parasaur on it & the bed next to it on the teleporter.\n"
            "When the lap is completed, the bot teleports back here.\n"
            "Ideally, you would have a trough here to keep your parasaur fed.";

        /**
         * @brief Callback to handle a new station being added to the parasaur.
         * Creates the default configuration for it and syncs it with the JSON.
         *
         * @param added The name of the new station that was added.
         */
        void on_added(const char* added)
        {
            auto& new_ = configs[added] = config::ManagedVar<
                bots::parasaur::ParasaurConfig>({"bots", "parasaur", added}, config::save,
                                                {});
            new_.save(); // Writes it to the JSON
        }

        /**
         * @brief Callback to handle a parasaur station being deleted.
         * Removes its configuration from the internal data and syncs it with the JSON.
         *
         * @param deleted The name of the parasaur station that was deleted.
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
            new_.set_path({"bots", "parasaur", new_name});
            auto& ref = configs[new_name] = new_;
            ref.save();

            // Now remove the old object
            configs[old_name].erase();
            configs.erase(old_name);
        }

        config::ManagedVar<bots::parasaur::ParasaurConfig>* get_active_station()
        {
            // No stations exist at all.
            if (stations.get_ref().empty()) { return nullptr; }
            const std::string name = stations.get_ref()[selected_station];

            // Make sure config for this station exists, if not just create a default.
            if (!configs.contains(name)) {
                configs[name] = config::ManagedVar<bots::parasaur::ParasaurConfig>(
                    {"bots", "parasaur", name}, config::save, {});
            };

            return &configs[name];
        }

        void load_mode_index()
        {
            if (has_loaded_mode) { return; }
            has_loaded_mode = true;

            const std::string curr = start_criteria.get();
            // not yet set, just use default (0)
            if (curr.empty()) { return; }

            if (const auto it = std::ranges::find(MODES, curr); it != MODES.end()) {
                selected_mode = static_cast<int>(std::distance(MODES.begin(), it));
            }
        }
    }

    void draw_parasaur_tab()
    {
        const ImVec2 stations_dimensions(425 - state::maintabs_data.width,
                                         ImGui::GetWindowHeight() * 0.33f);
        begin_child("Parasaur Stations", stations_dimensions);
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Station:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.7f);
            ImGui::SetCursorPos({80, 11});
            std::vector<const char*>* data = stations.get_ptr();

            auto flags = CustomComboFlags_Default | CustomComboFlags_AllowRename |
                CustomComboFlags_ConfirmRemove | CustomComboFlags_ButtonsBelow;

            const bool changed = custom::Combo("##selected_manager", &selected_station,
                                               *data, static_cast<CustomComboFlags_>(
                                                   flags), STATION_TOOLTIP,
                                               "Add New##parasaur",
                                               "Delete Current##parasaur",
                                               "Rename Current##parasaur", "Add parasaur",
                                               &add_dialog, &del_dialog, &rename_dialog,
                                               on_added, on_deleted, on_renamed);
            // Make sure we sync our manager in the config by saving any changes made.
            if (changed) { stations.save(); }
        }
        end_child();
        ImGui::SameLine();
        const ImVec2 advanced_dimensions(270, ImGui::GetWindowHeight() * 0.53f);
        begin_child("General Configuration", advanced_dimensions);
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Start bed:");

            ImGui::SetCursorPos({100, 11});
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f);

            if (ImGui::InputText("##parasaur_start", teleport_start.get_ptr())) {
                teleport_start.save();
            }
            if (ImGui::IsItemHovered()) { ImGui::SetTooltip(START_BED_TOOLTIP); }


            load_mode_index();
            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Start Mode:");

            ImGui::SetCursorPos({100, 42});
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f);
            if (ImGui::Combo("##parasaur_mode", &selected_mode, MODES.data(),
                             MODES.size())) { start_criteria.set(MODES[selected_mode]); }

            if (ImGui::IsItemHovered()) { ImGui::SetTooltip(MODE_TOOLTIP); }

            ImGui::SetCursorPos({10, 76});
            const char* text;
            config::ManagedVar<int>* dst = nullptr;

            if (strcmp(MODES[selected_mode], "INTERVAL") == 0) {
                text = "Interval:";
                dst = &start_interval;
            }
            else {
                text = "Min. Ready:";
                dst = &start_min_ready;
            }

            ImGui::Text(text);
            ImGui::SetCursorPos({100, 73});
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f);
            if (ImGui::InputInt("##interval|min", dst->get_ptr(), 1, 5)) { dst->save(); }

            if (ImGui::Checkbox("Disabled", disabled.get_ptr())) { disabled.save(); }
        }
        end_child();

        if (const auto active = get_active_station(); active != nullptr) {
            ImGui::SetCursorPosY((ImGui::GetWindowHeight() * 0.33f) + 5);
            const ImVec2 configuration_dimensions(stations_dimensions.x,
                                                  ImGui::GetWindowHeight() * 0.66f);
            begin_child("Configuration", configuration_dimensions);
            {
                ImGui::SetCursorPos({10, 14});
                ImGui::Text("Bed/Teleporter:");

                ImGui::SetCursorPos({130, 11});
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
                if (ImGui::InputText("##para_name", &active->get_ptr()->name)) {
                    active->save();
                }

                ImGui::SetCursorPos({10, 45});
                ImGui::Text("Interval (min):");
                ImGui::SetCursorPos({130, 42});
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
                if (ImGui::InputInt("##para_interval", &active->get_ptr()->interval)) {
                    active->save();
                }

                ImGui::SetCursorPos({10, 76});
                ImGui::Text("Load for (s):");
                ImGui::SetCursorPos({130, 73});
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
                if (ImGui::InputInt("##para_load", &active->get_ptr()->load_for)) {
                    active->save();
                }

                ImGui::SetCursorPos({10, 107});
                ImGui::Text("Alert Level:");
                ImGui::SetCursorPos({130, 104});
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
                if (ImGui::Combo("##alert_level", &active->get_ptr()->alert_level,
                                 ALERT_MODES.data(), ALERT_MODES.size())) {
                    active->save();
                }

                if (ImGui::Checkbox("Teleporter", &active->get_ptr()->is_teleporter)) {
                    active->save();
                }
                ImGui::SameLine();
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

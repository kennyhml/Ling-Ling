#include "paste.h"
#include "../common.h"
#include "../state.h"
#include "../../config/config.h"
#include "../../../external/imgui/imgui_stdlib.h"
#include "../custom/clampedint.h"

namespace llpp::gui
{
    using namespace config::bots::paste;


    const char* const RENDER_TOOLTIP =
        "Specifies the prefix for the render bed. For more details, please refer to the explanation below.\n\n"
        "Why is a render bed needed?\n"
        "When the snail is loaded before the structure it's on, it visually glitches into it.\n"
        "You need to render the structure, leave the render of the dino, and then move back to it.\n"
        "To achieve this, you'll require a source bed close to the snails named [PREFIX]::SRC\n"
        "and a gateway bed out of snail render named [PREFIX]::GATEWAY.";


    const char* const RENDER_TIME_TOOLTIP =
        "Specifies the duration for stations to render at the source render bed (in seconds).\n"
        "The rendering time increases with more structures; additional structures require a longer rendering time.";


    void draw_paste_tab()
    {
        begin_child("Station Configuration",
                    ImVec2(475 - state::maintabs_data.width,
                           ImGui::GetWindowHeight() * 0.52));
        {
            ImGui::SetCursorPos({10, 5});
            ImGui::Text("Station prefix:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 2});
            if (ImGui::InputText("##paste_prefix", prefix.get_ptr())) { prefix.save(); }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("Specifies the prefix for your paste beds.");
            }

            ImGui::SetCursorPos({10, 36});
            ImGui::Text("Render prefix:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 33});
            if (ImGui::InputText("##render_prefix", render_prefix.get_ptr())) {
                render_prefix.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(RENDER_TOOLTIP);
            }

            ImGui::SetCursorPos({10, 67});
            ImGui::Text("Station count:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 64});

            if (custom::ClampedInputInt("##paste_num", num_stations.get_ptr(), 1, 120)) {
                num_stations.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The number of paste stations you have.");
            }

            ImGui::SetCursorPos({10, 98});
            ImGui::Text("Interval (minutes):");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 95});
            if (custom::ClampedInputInt("##paste_interval", interval.get_ptr(), 5, 100)) {
                interval.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "The interval to complete the station at (in minutes).");
            }

            ImGui::SetCursorPos({10, 129});
            ImGui::Text("Load for (seconds):");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 126});
            if (custom::ClampedInputInt("##paste_render", load_for.get_ptr(), 5, 100)) {
                load_for.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(RENDER_TIME_TOOLTIP);
            }
        }
        end_child();

        ImGui::SameLine();
        begin_child("Advanced", ImVec2(280, ImGui::GetWindowHeight()));
        {
            ImGui::SetCursorPos({10, 11});
            if (ImGui::Checkbox("Disabled", disabled.get_ptr())) { disabled.save(); }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("Completely disable paste completion.");
            }
            if (ImGui::Checkbox("Allow partial completion", allow_partial.get_ptr())) {
                allow_partial.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("Allows the bot to complete other tasks in between.");
            }

            if (ImGui::Checkbox("OCR deposited amount", ocr_amount.get_ptr())) {
                ocr_amount.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("Use OCR to determine the paste deposited.");
            }

            if (ImGui::Checkbox("Mute Pings", mute_pings.get_ptr())) {
                mute_pings.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("Enable to mute discord pings for paste.");
            }
            end_child();
        }
    }
}

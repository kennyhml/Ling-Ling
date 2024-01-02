#include "custom.h"

#include <filesystem>
#include "../../external/imgui/imgui_internal.h"
#include "../config/config.h"

namespace llpp::gui
{
    namespace
    {
        int accent_color[4] = {140, 131, 214, 255};

        ImColor get_accent_color(float a = 1.f)
        {
            return {
                accent_color[0] / 255.f, accent_color[1] / 255.f, accent_color[2] / 255.f,
                a
            };
        }
    }


    bool tab_button(const char* icon, const char* label, const bool selected,
                    const float rounding, const ImDrawFlags flags)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        const ImGuiID id = window->GetID(label);

        const auto font = ImGui::GetIO().Fonts->Fonts[1];
        // get the size of the buttons text and icon
        const ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);
        const ImVec2 icon_size = font->CalcTextSizeA(font->FontSize - 2, FLT_MAX, 0,
                                                     icon);

        const ImVec2 start = window->DC.CursorPos;
        // Draw from the top left to the full width and 45 down
        const ImRect rect(start, start + ImVec2(ImGui::GetWindowWidth(), 45));
        ImGui::ItemAdd(rect, id);
        ImGui::ItemSize(rect, ImGui::GetStyle().FramePadding.y);

        // add button behavior instead of making it a button and check if its pressed
        bool hovered, held;
        const bool pressed = ImGui::ButtonBehavior(rect, id, &hovered, &held, NULL);

        static std::unordered_map<ImGuiID, float> saved;

        if (!saved.contains(id)) { saved[id] = 0.f; }
        float& anim = saved[id];

        anim = ImLerp(anim, (selected ? 1.f : 0.f), 0.035f);

        // Create the background colour of the 'button'
        const auto bg_colour = ImColor(1.f, 1.f, 1.f, 0.025f * anim);
        window->DrawList->AddRectFilled(rect.Min, rect.Max, bg_colour, rounding, flags);

        const ImVec2 icon_start(rect.Min.x + 25 - (icon_size.x / 2),
                                rect.GetCenter().y - icon_size.y / 2);
        const auto icon_col = ImGui::GetColorU32(ImGuiCol_Text,
                                                 (anim > 0.3f ? anim : 0.3f));
        window->DrawList->AddText(font, font->FontSize - 2, icon_start, icon_col, icon);

        const auto text_start = ImVec2(rect.Min.x + 50,
                                       rect.GetCenter().y - (label_size.y / 2) + 0.5f);
        const auto text_col = ImGui::GetColorU32(ImGuiCol_Text,
                                                 (anim > 0.3f ? anim : 0.3f));
        window->DrawList->AddText(text_start, text_col, label);

        return pressed;
    }

    bool sub_tab_button(const char* label, const bool selected)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        const ImGuiID id = window->GetID(label);

        const ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);
        const ImVec2 pos = window->DC.CursorPos;

        const ImRect rect(pos, pos + label_size);
        ImGui::ItemAdd(rect, id);
        ImGui::ItemSize(ImVec4(rect.Min.x, rect.Min.y, rect.Max.x + 15, rect.Max.y), 0);

        bool hovered, held;
        ImRect pressable_rect(rect.Min.x - 7, rect.Min.y - 7, rect.Max.x + 7,
                              rect.Max.y + 7);
        const bool pressed = ImGui::ButtonBehavior(pressable_rect, id, &hovered, &held,
                                                   NULL);

        static std::unordered_map<ImGuiID, float> saved;
        if (!saved.contains(id)) { saved[id] = 0.f; }
        float& anim = saved[id];

        anim = ImLerp(anim, (selected ? 1.f : 0.f), 0.035f);

        window->DrawList->AddText(rect.Min,
                                  ImGui::GetColorU32(ImGuiCol_Text, max(anim, 0.4f)),
                                  label);

        window->DrawList->AddLine(ImVec2(rect.Min.x - 2, rect.Max.y + 10),
                                  ImVec2(rect.Max.x + 2, rect.Max.y + 10),
                                  get_accent_color(anim * ImGui::GetStyle().Alpha), 2);
        return pressed;
    }


    void render_main_tab_area(const std::string& name, const ImVec2 size,
                              const std::function<void()>& render_content)
    {
        auto& [is_hovered, expand, width] = maintabs_data;

        // adjust the with depending on whether we are expanding or collapsing
        width = ImLerp(width, expand ? size.x + 60 : size.x, 0.08f);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::BeginChild(std::string(name).append(".child").c_str(),
                          ImVec2(width, size.y));

        is_hovered = ImGui::IsMouseHoveringRect(ImGui::GetCurrentWindow()->Pos,
                                                ImGui::GetCurrentWindow()->Pos + ImVec2(
                                                    width, size.y));

        ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetCurrentWindow()->Pos,
                                                  ImGui::GetCurrentWindow()->Pos +
                                                  ImGui::GetCurrentWindow()->Size,
                                                  ImColor(28, 30, 36),
                                                  ImGui::GetStyle().WindowRounding,
                                                  ImDrawFlags_RoundCornersTopLeft);

        if (tab_button(
            ICON_FA_MAGNIFYING_GLASS_ARROW_RIGHT, "Expand", expand,
            ImGui::GetStyle().WindowRounding, ImDrawFlags_RoundCornersTopLeft)) {
            expand = !expand;
        }

        render_content();
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }

    inline ImVec4 path_color = ImVec4(1.f, 0.f, 0.f, 1.f);
    inline bool bools[50]{};
    inline int ints[50]{};
    inline float color[4] = {1.f, 1.f, 1.f, 1.f};

    void draw_general_ark_tabs()
    {
        begin_child("Game Settings", ImVec2(300, ImGui::GetWindowHeight()));
        {
            // CONFIG FOR THE ARK ROOT DIRECTORY
            ImGui::SetCursorPos({10, 14});
            ImGui::TextColored(path_color, "Directory:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({100, 11});
            ImGui::InputText("##", config::general::ark::root_dir.get().data(), 256,
                             ImGuiInputTextFlags_CallbackEdit,
                             [](ImGuiInputTextCallbackData* data) -> int {
                                 config::general::ark::root_dir.set(data->Buf);
                                 return 0;
                             });
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The path to the root game directory.");
            }

            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
            if (ImGui::Button("Get")) {
                std::string selected;
                openFolder(selected);
                if (!selected.empty()) { config::general::ark::root_dir.set(selected); }
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("Select the directory.");
            }
            const bool valid = exists(
                std::filesystem::path(config::general::ark::root_dir.get()));
            path_color = valid ? ImVec4(0.f, 1.f, 0.f, 1.f) : ImVec4(1.f, 0.f, 0.f, 1.f);

            // CONFIG FOR THE ARK ROOT DIRECTORY
            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Server:");
            ImGui::SetCursorPos({100, 42});
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::InputText("##server", config::general::ark::server.get().data(), 256,
                             ImGuiInputTextFlags_CallbackEdit,
                             [](ImGuiInputTextCallbackData* data) -> int {
                                 config::general::ark::server.set(data->Buf);
                                 return 0;
                             });
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("Your server name.");
            }
        }
        end_child();

        ImGui::SameLine();
        begin_child("f", ImVec2(280, ImGui::GetWindowHeight()));
        end_child();
    }

    inline ImVec4 assets_path_col = ImVec4(1.f, 0.f, 0.f, 1.f);
    inline ImVec4 itemdata_col = ImVec4(1.f, 0.f, 0.f, 1.f);

    void draw_general_bot_tabs()
    {
        begin_child("Environment", ImVec2(375, ImGui::GetWindowHeight()));
        {
            // CONFIG FOR THE ARK ROOT DIRECTORY
            ImGui::SetCursorPos({10, 14});
            ImGui::TextColored(assets_path_col, "Assets directory:");

            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.55f);
            ImGui::SetCursorPos({120, 10});
            ImGui::InputText("##assets_dir",
                             config::general::bot::assets_dir.get().data(), 256,
                             ImGuiInputTextFlags_CallbackEdit,
                             [](ImGuiInputTextCallbackData* data) -> int {
                                 config::general::bot::assets_dir.set(data->Buf);
                                 return 0;
                             });
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The path to the assets shipped with ling ling.");
            }

            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
            ImGui::SetCursorPosY(10);
            if (ImGui::Button("Get")) {
                std::string selected;
                openFolder(selected);
                if (!selected.empty()) { config::general::bot::assets_dir.set(selected); }
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("Select the directory.");
            }
            bool valid = std::filesystem::exists(config::general::bot::assets_dir.get())
                || exists(
                    std::filesystem::current_path() / config::general::bot::assets_dir.
                    get());
            assets_path_col = valid
                                  ? ImVec4(0.f, 1.f, 0.f, 1.f)
                                  : ImVec4(1.f, 0.f, 0.f, 1.f);


            // CONFIG FOR THE ARK ROOT DIRECTORY
            ImGui::SetCursorPos({10, 45});
            ImGui::TextColored(itemdata_col, "Itemdata path:");

            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.55f);
            ImGui::SetCursorPos({120, 42});
            ImGui::InputText("##itemdata", config::general::bot::itemdata.get().data(),
                             256, ImGuiInputTextFlags_CallbackEdit,
                             [](ImGuiInputTextCallbackData* data) -> int {
                                 config::general::bot::itemdata.set(data->Buf);
                                 return 0;
                             });
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The path to the itemdata shipped with ling ling.");
            }

            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
            ImGui::SetCursorPosY(10);
            if (ImGui::Button("Get")) {
                std::string selected;
                openFolder(selected);
                if (!selected.empty()) { config::general::bot::itemdata.set(selected); }
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("Select the directory.");
            }
            valid = std::filesystem::exists(config::general::bot::itemdata.get()) ||
                exists(
                    std::filesystem::current_path() / config::general::bot::itemdata.
                    get());
            itemdata_col = valid
                               ? ImVec4(0.f, 1.f, 0.f, 1.f)
                               : ImVec4(1.f, 0.f, 0.f, 1.f);
        }
        end_child();

        ImGui::SameLine();
        begin_child("f", ImVec2(205, ImGui::GetWindowHeight()));
        end_child();
    }


    inline int selected_channel = 0;
    inline int selected_role = 0;
    inline int selected_user = 0;
    inline bool new_channel_popup = false;
    inline bool new_role_popup = false;
    inline bool new_user_popup = false;

    void draw_discord_bot_config()
    {
        begin_child("Bot settings", ImVec2(375, ImGui::GetWindowHeight()));
        {
            // TOKEN SHITS
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Discord Bot token:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.55f);
            ImGui::SetCursorPos({150, 11});
            ImGui::InputText("##bot_token", config::discord::token.get().data(), 256,
                             ImGuiInputTextFlags_Password |
                             ImGuiInputTextFlags_CallbackEdit,
                             [](ImGuiInputTextCallbackData* data) -> int {
                                 config::discord::token.set(data->Buf);
                                 return 0;
                             });

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "The token for your discord bot, you must create it yourself.\nDO NOT SHARE THIS WITH ANYONE.");
            }


            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Discord (Guild) ID:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.55f);
            ImGui::SetCursorPos({150, 42});
            ImGui::InputText("##bot_guild_id", config::discord::guild.get().data(), 256,
                             ImGuiInputTextFlags_CallbackEdit |
                             ImGuiInputTextFlags_CharsDecimal,
                             [](ImGuiInputTextCallbackData* data) -> int {
                                 config::discord::guild.set(data->Buf);
                                 return 0;
                             });

            // COMMAND CHANNEL SHITS
            ImGui::SetCursorPos({10, 76});
            ImGui::Text("Command channels:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.41f);
            ImGui::SetCursorPos({150, 73});
            auto* channel_data = config::discord::authorization::channels.get_ptr();
            ImGui::Combo("##command_channels", &selected_channel, channel_data->data(),
                         channel_data->size());

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "A list of channel IDs where bot commands are permitted; roles and users are still respected.\n"
                    "Leave this field empty to allow bot commands to be used in any channel.");
            }

            ImGui::SameLine();
            if (ImGui::Button("+##channel")) { new_channel_popup = true; }
            ImGui::SameLine();
            if (ImGui::Button("-##channel") && !channel_data->empty()) {
                channel_data->erase(channel_data->begin() + selected_channel);
                config::discord::authorization::channels.save();
                selected_channel = max(0, selected_channel);
            }

            if (new_channel_popup) {
                ImGui::OpenPopup("Add an authorized command channel");
                if (ImGui::BeginPopupModal("Add an authorized command channel",
                                           &new_channel_popup,
                                           ImGuiWindowFlags_AlwaysAutoResize)) {
                    static char name_buffer[256] = {};
                    if (ImGui::IsWindowAppearing()) {
                        memset(name_buffer, 0, sizeof(name_buffer));
                    }

                    ImGui::InputText("##input_command_channel", name_buffer,
                                     IM_ARRAYSIZE(name_buffer),
                                     ImGuiInputTextFlags_CharsDecimal);
                    if (ImGui::Button("OK")) {
                        new_channel_popup = false;
                        channel_data->push_back(_strdup(name_buffer));
                        config::discord::authorization::channels.save();
                        selected_channel = channel_data->size() - 1;
                    }
                    ImGui::EndPopup();
                }
            }


            // ROLES SHITS
            ImGui::SetCursorPos({10, 107});
            ImGui::Text("Authorized roles:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.41f);
            ImGui::SetCursorPos({150, 104});
            auto* roles_data = config::discord::authorization::roles.get_ptr();
            ImGui::Combo("##authed_roles", &selected_role, roles_data->data(),
                         roles_data->size());

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "A list of role IDs authorized to execute bot commands.\n");
            }

            ImGui::SameLine();
            if (ImGui::Button("+##role")) { new_role_popup = true; }
            ImGui::SameLine();
            if (ImGui::Button("-##role") && !roles_data->empty()) {
                roles_data->erase(roles_data->begin() + selected_role);
                config::discord::authorization::roles.save();
                selected_role = max(0, selected_role - 1);
            }

            if (new_role_popup) {
                ImGui::OpenPopup("Add an authorized role");
                if (ImGui::BeginPopupModal("Add an authorized role", &new_role_popup,
                                           ImGuiWindowFlags_AlwaysAutoResize)) {
                    static char name_buffer[256] = {};
                    if (ImGui::IsWindowAppearing()) {
                        memset(name_buffer, 0, sizeof(name_buffer));
                    }

                    ImGui::InputText("##input_role", name_buffer,
                                     IM_ARRAYSIZE(name_buffer),
                                     ImGuiInputTextFlags_CharsDecimal);
                    if (ImGui::Button("OK")) {
                        new_role_popup = false;
                        roles_data->push_back(_strdup(name_buffer));
                        config::discord::authorization::roles.save();
                        selected_role = roles_data->size() - 1;
                    }
                    ImGui::EndPopup();
                }
            }

            // USERS SHITS
            ImGui::SetCursorPos({10, 138});
            ImGui::Text("Authorized users:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.41f);
            ImGui::SetCursorPos({150, 135});
            auto* user_data = config::discord::authorization::users.get_ptr();
            ImGui::Combo("##authed_users", &selected_user, user_data->data(),
                         user_data->size());

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "A list of user IDs authorized to execute bot commands.\n"
                    "Takes priority over roles; for example, a specified user does not require an authorized role.");
            }

            ImGui::SameLine();
            if (ImGui::Button("+##user")) { new_user_popup = true; }
            ImGui::SameLine();
            if (ImGui::Button("-##user") && !user_data->empty()) {
                user_data->erase(user_data->begin() + selected_user);
                config::discord::authorization::users.save();
                selected_user = max(0, selected_user - 1);
            }

            if (new_user_popup) {
                ImGui::OpenPopup("Add an authorized role");
                if (ImGui::BeginPopupModal("Add an authorized role", &new_user_popup,
                                           ImGuiWindowFlags_AlwaysAutoResize)) {
                    static char name_buffer[256] = {};
                    if (ImGui::IsWindowAppearing()) {
                        memset(name_buffer, 0, sizeof(name_buffer));
                    }

                    ImGui::InputText("##input_role", name_buffer,
                                     IM_ARRAYSIZE(name_buffer),
                                     ImGuiInputTextFlags_CharsDecimal);
                    if (ImGui::Button("OK")) {
                        new_user_popup = false;
                        user_data->push_back(_strdup(name_buffer));
                        config::discord::authorization::users.save();
                        selected_user = user_data->size() - 1;
                    }
                    ImGui::EndPopup();
                }
            }


            end_child();
            ImGui::SameLine();
            begin_child("Advanced", ImVec2(205, ImGui::GetWindowHeight()));
            {
                ImGui::SetCursorPos({10, 11});
                if (ImGui::Checkbox("Use ephemeral replies",
                                    config::discord::advanced::ephemeral_replies.
                                    get_ptr())) {
                    config::discord::advanced::ephemeral_replies.save();
                }
            }
            end_child();
        }
    }

    void draw_discord_info_tabs()
    {
        begin_child("Roles / People", ImVec2(300, ImGui::GetWindowHeight() / 2));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Helpers [No Access]:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 11});
            ImGui::InputText("##helpers_no_access",
                             config::discord::roles::helper_no_access.get().data(), 256,
                             ImGuiInputTextFlags_CallbackEdit,
                             [](ImGuiInputTextCallbackData* event)-> int {
                                 config::discord::roles::helper_no_access.set(event->Buf);
                                 return 0;
                             });
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "This role will be mentioned for any problems ling ling encounters\nthat can be fixed by someone without direct access to ling ling.\n"
                    "Examples:\n-An Achatina is missing or inaccessible.\n-The bed/teleporter of a task is missing.\n-The vault of a task is capped / close to cap.");
            }

            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Helpers [Access]:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 42});
            ImGui::InputText("##helpers_access",
                             config::discord::roles::helper_access.get().data(), 256,
                             ImGuiInputTextFlags_CallbackEdit,
                             [](ImGuiInputTextCallbackData* event)-> int {
                                 config::discord::roles::helper_access.set(event->Buf);
                                 return 0;
                             });
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "This role will be mentioned for any fatal problems ling ling encounters\n and require direct access to be fixed."
                    "Examples:\n-An unexpected error at any task.\n-Failure to reconnect / restart .\n-Upcoming game update.");
            }
        }
        end_child();

        ImGui::SameLine();
        begin_child("Advanced", ImVec2(280, ImGui::GetWindowHeight())); {}
        end_child();

        ImGui::SetCursorPosY((ImGui::GetWindowHeight() / 2) + 5);
        begin_child("Channels", ImVec2(300, ImGui::GetWindowHeight() / 2));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Info Channel:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 11});
            ImGui::InputText("##info_channel",
                             config::discord::channels::info.get().data(), 256,
                             ImGuiInputTextFlags_CallbackEdit,
                             [](ImGuiInputTextCallbackData* event)-> int {
                                 config::discord::channels::info.set(event->Buf);
                                 return 0;
                             });
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "[REQUIRED] - General info will be posted here (stations completed, times taken...)");
            }

            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Error Channel:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 42});
            ImGui::InputText("##error_channel",
                             config::discord::channels::error.get().data(), 256,
                             ImGuiInputTextFlags_CallbackEdit,
                             [](ImGuiInputTextCallbackData* event)-> int {
                                 config::discord::channels::error.set(event->Buf);
                                 return 0;
                             });
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "[OPTIONAL] - Errors and warnings will be posted here (game crashed, vaults full..)\nIf empty these messages will fall back to the info channel.");
            }
        }
        end_child();
    }

    void draw_bots_paste_tabs()
    {
        begin_child("Station Configuration", ImVec2(300, ImGui::GetWindowHeight()));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Station prefix:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 11});
            ImGui::InputText("##paste_prefix", config::bots::paste::prefix.get().data(),
                             256, ImGuiInputTextFlags_CallbackEdit,
                             [](ImGuiInputTextCallbackData* event)-> int {
                                 config::bots::paste::prefix.set(event->Buf);
                                 return 0;
                             });
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "Specifies the prefix for your paste beds. The prefix must be included in your bed name but may not be identical.\n"
                    "For instance, your bed could be named COOL SPOT // PASTE00, while your prefix can still be PASTE.");
            }

            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Render prefix:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 42});
            ImGui::InputText("##render_prefix",
                             config::bots::paste::render_prefix.get().data(), 256,
                             ImGuiInputTextFlags_CallbackEdit,
                             [](ImGuiInputTextCallbackData* event)-> int {
                                 config::bots::paste::render_prefix.set(event->Buf);
                                 return 0;
                             });
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "Specifies the prefix for the render bed. For more details, please refer to the explanation below.\n\n"
                    "Why is a render bed needed?\n"
                    "When the snail is loaded before the structure it's on, it visually glitches into it.\n"
                    "To resolve this issue, you need to render the structure, leave the render of the dino, and then move back to it.\n"
                    "To achieve this, you'll require a source bed close to the snails named [PREFIX]::SRC\n"
                    "and a gateway bed out of snail render named [PREFIX]::GATEWAY.");
            }

            ImGui::SetCursorPos({10, 76});
            ImGui::Text("Station count:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 73});
            if (ImGui::InputInt("##paste_count",
                                config::bots::paste::num_stations.get_ptr(), 1, 5)) {
                config::bots::paste::num_stations.save();
            }
            int* num_stations = config::bots::paste::num_stations.get_ptr();
            *num_stations = std::clamp(*num_stations, 1, 100);

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The number of paste stations you have.");
            }


            ImGui::SetCursorPos({10, 107});
            ImGui::Text("Interval (minutes):");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 104});
            if (ImGui::InputInt("##paste_interval",
                                config::bots::paste::interval.get_ptr(), 1, 5)) {
                config::bots::paste::interval.save();
            }
            int* interval = config::bots::paste::interval.get_ptr();
            *interval = std::clamp(*interval, 5, 150);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "The interval to complete the station at (in minutes).");
            }


            ImGui::SetCursorPos({10, 138});
            ImGui::Text("Load for (seconds):");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 135});
            if (ImGui::InputInt("##paste_render", config::bots::paste::load_for.get_ptr(),
                                1, 5)) { config::bots::paste::load_for.save(); }
            int* load_for = config::bots::paste::load_for.get_ptr();
            *load_for = std::clamp(*load_for, 5, 100);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "Specifies the duration for stations to render at the source render bed (in seconds).\n"
                    "The rendering time increases with more structures; additional structures require a longer rendering time.");
            }
        }
        end_child();

        ImGui::SameLine();
        begin_child("Advanced", ImVec2(280, ImGui::GetWindowHeight()));
        {
            ImGui::SetCursorPos({10, 11});
            if (ImGui::Checkbox("Disable station completion",
                                config::bots::paste::disable_completion.get_ptr())) {
                config::bots::paste::disable_completion.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "Completely disable the paste manager's completion.\n\n"
                    "Even when disabled, the paste manager is created but remains inactive.\n"
                    "You can toggle its state at runtime or through the discord bot.");
            }

            if (ImGui::Checkbox("Allow partial completion",
                                config::bots::paste::allow_partial.get_ptr())) {
                config::bots::paste::allow_partial.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "Enables the bot to pause its operation after reaching a specific station and resume afterward.\n\n"
                    "For example:\nSuppose there are Task A, Task B, and Task C in order of priority. Task C represents\n"
                    "a PasteManager instance handling paste stations. If both Task A and B are on cooldown and C is initiated,\n"
                    "A and B will be temporarily ignored until C completes. In cases of partial completion, C might finish\n"
                    "station 3, execute Task A/B, and then resume C at station 4.\n\n"
                    "Not recommended if there will be frequent partial completions and rendering takes a considerable amount of time.\n");
            }


            if (ImGui::Checkbox("OCR deposited amount",
                                config::bots::paste::ocr_amount.get_ptr())) {
                config::bots::paste::ocr_amount.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "Enable to use OCR to determine how much paste was put into the dedi per station.\n"
                    "This info is sent to the completion embed on discord, it has no other purpose as of now.");
            }

            end_child();
        }
    }


    inline int selected_manager = 0;
    inline bool new_name_popup = false;
    inline bool confirmation_popup = false;

    inline int num2 = 0;
    char dir_buffer[256];

    void draw_bots_drops_tab()
    {
        begin_child("Crate Managers", ImVec2(300, ImGui::GetWindowHeight() * 0.33));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Selected Manager:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 11});
            std::vector<const char*>* data = config::bots::drops::managers.get_ptr();
            ImGui::Combo("##selected_manager", &selected_manager, data->data(),
                         data->size());

            ImGui::SetCursorPos({5, 45});
            if (ImGui::Button("Create manager")) { new_name_popup = true; }
            ImGui::SameLine();
            if (ImGui::Button("Delete selected manager") && !data->empty()) {
                confirmation_popup = true;
            }

            if (new_name_popup) {
                ImGui::OpenPopup("Name your new crate manager");
                if (ImGui::BeginPopupModal("Name your new crate manager", &new_name_popup,
                                           ImGuiWindowFlags_AlwaysAutoResize)) {
                    static char name_buffer[256] = {};
                    if (ImGui::IsWindowAppearing()) {
                        memset(name_buffer, 0, sizeof(name_buffer));
                    }

                    ImGui::InputText("##InputText", name_buffer,
                                     IM_ARRAYSIZE(name_buffer));
                    if (ImGui::Button("OK")) {
                        new_name_popup = false;
                        data->push_back(_strdup(name_buffer));
                        config::bots::drops::managers.save();
                    }
                    ImGui::EndPopup();
                }
            }

            if (confirmation_popup) {
                const auto name = (*data)[selected_manager];
                const std::string title = "Delete '" + std::string(name) + "'";
                ImGui::OpenPopup(title.c_str());

                if (ImGui::BeginPopupModal(title.c_str(), &confirmation_popup,
                                           ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("Are you sure about this?\nThis cannot be undone!");
                    if (ImGui::Button("OK", ImVec2(80, 0))) {
                        printf("Action confirmed!\n");
                        confirmation_popup = false;
                        data->erase(data->begin() + selected_manager);
                        config::bots::drops::managers.save();
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Cancel", ImVec2(80, 0))) {
                        printf("Action canceled!\n");
                        confirmation_popup = false;
                    }
                    ImGui::EndPopup();
                }
            }
        }
        end_child();

        ImGui::SameLine();
        begin_child("Advanced", ImVec2(280, ImGui::GetWindowHeight()));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Render (seconds):");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 11});
            ImGui::InputInt("##crate_render", &num2, 1, 5);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "How long to let the first crate at each group render, more structures nearby -> longer render time.");
            }
            num2 = (num2 < 0) ? 0 : num2;
            if (num2 > 15) { num2 = 15; }

            ImGui::SetCursorPos({10, 45});
            ImGui::Checkbox("Overrule reroll mode", &bools[0]);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "Enable to always loot these crates, even when it is enabled.");
            }
            ImGui::SetCursorPos({10, 76});
            ImGui::Checkbox("Allow partial completion", &bools[0]);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "[ONLY SUPPORTED FOR BED STATIONS]\n\n"
                    "Allow the bot to break after a certain station and pick back up after that.\n\n"
                    "Example:\nConsider we have Task A, Task B and Task C where the order is their priority and C is\n"
                    "our CrateManager instance managing the crates.\nIf Task A and B are both on cooldown and C is started, A and B will be disregarded\n"
                    "until C completes. With partial completion, C may go to complete station 3, complete task A/B,\n"
                    "then pick C back up at 4.");
            }
        }
        end_child();

        ImGui::SetCursorPosY((ImGui::GetWindowHeight() * 0.33) + 5);
        begin_child("Configuration", ImVec2(300, ImGui::GetWindowHeight() * 0.66));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Station prefix:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 11});
            ImGui::InputText("##crate_prefix", dir_buffer, 256);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "The prefix of this crate manager.\n\n"
                    "The following structure instances will be created with the prefix:\n"
                    "-[PREFIX]::ALIGN - The initial bed to spawn on for teleporter mode\n"
                    "-[PREFIX]::DROPXX - The teleporter / bed per drop where XX is it's index\n"
                    "-[PREFIX]::DROPOFF - The vault the items will be stored in for teleporter mode\n");
            }

            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Grouped Crates:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 42});
            ImGui::InputText("##crate_groups", dir_buffer, 256);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "An array of arrays where each array represents one group of crates.\n"
                    "Each group corresponds to crates that share the same uptime,\n"
                    "meaning only one of them will be active at any given time (e.g., Swamp Cave big room).\n\n"
                    "Guidelines:\n" " - Enclose each group in curly braces { and }.\n"
                    " - Separate elements within a group with commas.\n"
                    " - For each crate 'j' per crates[i], provide one bed or tp with the corresponding name.\n"
                    " - Define each crate with its color options: BLUE, YELLOW, RED.\n"
                    " - Use a pipe '|' to separate multiple options for a crate (e.g., YELLOW | RED).\n"
                    " - 'ANY' can be used to represent multiple color options for a crate.\n\n"
                    "Example for Island Swamp Cave:\n"
                    "{RED, RED}, {YELLOW, YELLOW, ANY}, {BLUE}");
            }

            ImGui::SetCursorPos({10, 76});
            ImGui::Text("Interval (minutes):");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 73});
            ImGui::InputInt("##crate_interval", &num2, 1, 5);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                    "The interval to complete the stations at (in minutes).");
            }
            num2 = (num2 <= 5) ? 5 : num2;
            if (num2 > 100) { num2 = 100; }

            ImGui::SetCursorPos({10, 107});
            ImGui::Checkbox("Uses teleporters", &bools[5]);
        }
        end_child();
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
                                                          36 / 255.f, animation), 4);
        ImGui::GetWindowDrawList()->AddText(pos + ImVec2(10, 5),
                                            get_accent_color(animation), name,
                                            ImGui::FindRenderedTextEnd(name));

        ImGui::SetCursorPosY(30);
        ImGui::BeginChild(name, {size.x, size.y - 30});
        ImGui::SetCursorPosX(10);

        ImGui::BeginGroup();

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, animation);
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

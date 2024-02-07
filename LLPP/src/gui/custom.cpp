#include "custom.h"

#include "../../external/imgui/imgui_internal.h"
#include "../../external/imgui/imgui_stdlib.h"
#include <filesystem>

#include "../config/config.h"

namespace llpp::gui
{
    namespace
    {
        std::string spark_tooltip;
        std::string gp_tooltip;

        int accent_color[4] = {140, 131, 214, 255};

        ImColor get_accent_color(float a = 1.f)
        {
            return {
                    accent_color[0] / 255.f, accent_color[1] / 255.f,
                    accent_color[2] / 255.f,
                    a
            };
        }

        bool open_folder(std::string& folder_path_out)
        {
            // CREATE FILE OBJECT INSTANCE
            HRESULT f_SysHr = CoInitializeEx(
                    nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
            if (FAILED(f_SysHr)) { return FALSE; }

            // CREATE FileOpenDialog OBJECT
            IFileOpenDialog* f_FileSystem;
            f_SysHr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
                                       IID_IFileOpenDialog,
                                       reinterpret_cast<void**>(&f_FileSystem));
            if (FAILED(f_SysHr)) {
                CoUninitialize();
                return FALSE;
            }

            // SET OPTIONS FOR FOLDER SELECTION
            DWORD dwOptions;
            f_SysHr = f_FileSystem->GetOptions(&dwOptions);
            if (SUCCEEDED(f_SysHr)) {
                f_SysHr = f_FileSystem->SetOptions(dwOptions | FOS_PICKFOLDERS);
            }
            if (FAILED(f_SysHr)) {
                f_FileSystem->Release();
                CoUninitialize();
                return FALSE;
            }

            // SHOW OPEN FILE DIALOG WINDOW
            f_SysHr = f_FileSystem->Show(nullptr);
            if (FAILED(f_SysHr)) {
                f_FileSystem->Release();
                CoUninitialize();
                return FALSE;
            }

            // RETRIEVE FOLDER PATH FROM THE SELECTED ITEM
            IShellItem* f_Folder;
            f_SysHr = f_FileSystem->GetResult(&f_Folder);
            if (FAILED(f_SysHr)) {
                f_FileSystem->Release();
                CoUninitialize();
                return FALSE;
            }

            // STORE AND CONVERT THE FOLDER PATH
            PWSTR f_Path;
            f_SysHr = f_Folder->GetDisplayName(SIGDN_FILESYSPATH, &f_Path);
            if (FAILED(f_SysHr)) {
                f_Folder->Release();
                f_FileSystem->Release();
                CoUninitialize();
                return FALSE;
            }

            // FORMAT AND STORE THE FOLDER PATH
            std::wstring path(f_Path);
            std::string c(path.begin(), path.end());
            folder_path_out = c;

            // SUCCESS, CLEAN UP
            CoTaskMemFree(f_Path);
            f_Folder->Release();
            f_FileSystem->Release();
            CoUninitialize();
            return TRUE;
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
        const ImVec2 icon_size = font->CalcTextSizeA(
                font->FontSize - 2, FLT_MAX, 0, icon);

        const ImVec2 start = window->DC.CursorPos;
        // Draw from the top left to the full width and 45 down
        const ImRect rect(start, start + ImVec2(ImGui::GetWindowWidth(), 45));
        ImGui::ItemAdd(rect, id);
        ImGui::ItemSize(rect, ImGui::GetStyle().FramePadding.y);

        // add button behavior instead of making it a button and check if its
        // pressed
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

        if (tab_button(ICON_FA_MAGNIFYING_GLASS_ARROW_RIGHT, "Expand", expand,
                       ImGui::GetStyle().WindowRounding,
                       ImDrawFlags_RoundCornersTopLeft)) { expand = !expand; }

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

            if (ImGui::InputText("##root_dir",
                                 config::general::ark::root_dir.get_ptr())) {
                config::general::ark::root_dir.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The path to the root game directory.");
            }

            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
            if (ImGui::Button("Get")) {
                std::string selected;
                open_folder(selected);
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
            if (ImGui::InputText("##server", config::general::ark::server.get_ptr())) {
                config::general::ark::server.save();
            }
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
    inline ImVec4 tessdata_col = ImVec4(1.f, 0.f, 0.f, 1.f);

    void draw_general_bot_tabs()
    {
        begin_child("Environment", ImVec2(375, ImGui::GetWindowHeight() * 0.5));
        {
            // CONFIG FOR THE ARK ROOT DIRECTORY
            ImGui::SetCursorPos({10, 14});
            ImGui::TextColored(assets_path_col, "Assets directory:");

            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.55f);
            ImGui::SetCursorPos({120, 10});
            if (ImGui::InputText("##assets_dir",
                                 config::general::bot::assets_dir.get_ptr())) {
                config::general::bot::assets_dir.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The path to the assets shipped with ling ling.");
            }

            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
            ImGui::SetCursorPosY(10);
            if (ImGui::Button("Get")) {
                std::string selected;
                open_folder(selected);
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
            if (ImGui::InputText("##itemdata",
                                 config::general::bot::itemdata.get_ptr())) {
                config::general::bot::itemdata.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The path to the itemdata shipped with ling ling.");
            }
            valid = std::filesystem::exists(config::general::bot::itemdata.get()) ||
                    exists(std::filesystem::current_path() /
                           config::general::bot::itemdata.get());
            itemdata_col = valid
                           ? ImVec4(0.f, 1.f, 0.f, 1.f)
                           : ImVec4(1.f, 0.f, 0.f, 1.f);

            // CONFIG FOR THE ARK ROOT DIRECTORY
            ImGui::SetCursorPos({10, 76});
            ImGui::TextColored(tessdata_col, "Tessdata path:");

            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.55f);
            ImGui::SetCursorPos({120, 73});
            if (ImGui::InputText("##tessdata",
                                 config::general::bot::tessdata_dir.get_ptr())) {
                config::general::bot::tessdata_dir.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The path to the tessdata shipped with ling ling.");
            }
            valid = std::filesystem::exists(config::general::bot::tessdata_dir.get()) ||
                    exists(std::filesystem::current_path() /
                           config::general::bot::tessdata_dir.get());
            tessdata_col = valid
                           ? ImVec4(0.f, 1.f, 0.f, 1.f)
                           : ImVec4(1.f, 0.f, 0.f, 1.f);
        }
        end_child();

        ImGui::SameLine();
        begin_child("...", ImVec2(205, ImGui::GetWindowHeight()));
        {}
        end_child();

        ImGui::SetCursorPosY((ImGui::GetWindowHeight() * 0.5) + 5);
        begin_child("Additional Configuration",
                    ImVec2(375, ImGui::GetWindowHeight() * 0.5));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::TextColored(assets_path_col, "Commands:");

            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.55f);
            ImGui::SetCursorPos({120, 10});
            if (ImGui::InputText("##commands",
                                 config::general::bot::commands.get_ptr())) {
                config::general::bot::commands.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "The commands ling ling uses when starting or reconnecting.");
            }
        }
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
        begin_child("Bot settings",
                    ImVec2(475 - maintabs_data.width, ImGui::GetWindowHeight()));
        {
            // TOKEN SHITS
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Discord Bot token:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.55f);
            ImGui::SetCursorPos({150, 11});
            if (ImGui::InputText("##bot_token", config::discord::token.get_ptr(),
                                 ImGuiInputTextFlags_Password)) {
                config::discord::token.save();
            }

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "The token for your discord bot, you must create it "
                        "yourself.\nDO NOT SHARE THIS WITH ANYONE.");
            }


            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Discord (Guild) ID:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.55f);
            ImGui::SetCursorPos({150, 42});
            if (ImGui::InputText("##guild_id", config::discord::guild.get_ptr())) {
                config::discord::guild.save();
            }

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
                        "A list of channel IDs where bot commands are permitted; "
                        "roles and users are still respected.\n"
                        "Leave this field empty to allow bot commands to be used "
                        "in any channel.");
            }

            ImGui::SameLine();
            if (ImGui::Button("+##channel")) { new_channel_popup = true; }
            ImGui::SameLine();
            if (ImGui::Button("-##channel") && !channel_data->empty()) {
                channel_data->erase(channel_data->begin() + selected_channel);
                config::discord::authorization::channels.save();
                selected_channel = max(0, selected_channel - 1);
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
                        "Takes priority over roles; for example, a specified user "
                        "does not require an authorized role.");
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
        begin_child("Roles / People",
                    ImVec2(475 - maintabs_data.width, ImGui::GetWindowHeight() / 2));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Helpers [No Access]:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 11});
            if (ImGui::InputText("##helpers_no_access",
                                 config::discord::roles::helper_no_access.get_ptr())) {
                config::discord::roles::helper_no_access.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "This role will be mentioned for any problems ling ling "
                        "encounters\nthat can be fixed by someone without direct "
                        "access to ling ling.\n"
                        "Examples:\n-An Achatina is missing or inaccessible.\n-The "
                        "bed/teleporter of a task is missing.\n-The vault of a "
                        "task is capped / close to cap.");
            }

            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Helpers [Access]:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 42});
            if (ImGui::InputText("##helpers_access",
                                 config::discord::roles::helper_access.get_ptr())) {
                config::discord::roles::helper_access.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "This role will be mentioned for any fatal problems ling "
                        "ling encounters\n and require direct access to be fixed."
                        "Examples:\n-An unexpected error at any task.\n-Failure to "
                        "reconnect / restart .\n-Upcoming game update.");
            }
        }
        end_child();

        ImGui::SameLine();
        begin_child("Advanced", ImVec2(280, ImGui::GetWindowHeight()));
        {}
        end_child();

        ImGui::SetCursorPosY((ImGui::GetWindowHeight() / 2) + 5);
        begin_child("Channels",
                    ImVec2(475 - maintabs_data.width, ImGui::GetWindowHeight() / 2));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Info Channel:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 11});
            if (ImGui::InputText("##info_channel",
                                 config::discord::channels::info.get_ptr())) {
                config::discord::channels::info.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "[REQUIRED] - General info will be posted "
                        "here (stations completed, times taken...)");
            }

            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Error Channel:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 42});
            if (ImGui::InputText("##error_channel",
                                 config::discord::channels::error.get_ptr())) {
                config::discord::channels::error.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "[OPTIONAL] - Errors and warnings will be posted here "
                        "(game crashed, vaults full..)\nIf empty these messages "
                        "will fall back to the info channel.");
            }
        }
        end_child();
    }

    inline int selected_ignore_filter = 0;
    inline int selected_ping_filter = 0;

    inline bool new_ignore_popup = false;
    inline bool new_ping_popup = false;

    void draw_discord_alert_tabs()
    {
        begin_child("Configuration",
                    ImVec2(475 - maintabs_data.width, ImGui::GetWindowHeight() / 2));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Ping cooldown (s):");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 11});
            if (ImGui::InputInt("##ping_cooldown",
                                config::discord::alert_rules::ping_cooldown.get_ptr())) {
                config::discord::alert_rules::ping_cooldown.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The cooldown between any sort of alert pings.");
            }

            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Ping min. events:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 42});
            if (ImGui::InputInt("##ping_min_events",
                                config::discord::alert_rules::ping_min_events.get_ptr())) {
                config::discord::alert_rules::ping_min_events.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "The minimum amount of events within one update to allow pings.");
            }


            ImGui::SetCursorPos({10, 76});
            ImGui::Text("Suppress filter:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.31f);
            ImGui::SetCursorPos({150, 73});
            auto* ignore_data = config::discord::alert_rules::ignore_filter.get_ptr();
            ImGui::Combo("##ignore_filter", &selected_ignore_filter, ignore_data->data(),
                         ignore_data->size());

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "An alert message containing any of these terms will be ignored.");
            }

            ImGui::SameLine();
            if (ImGui::Button(" - ##ignore_filter") && !ignore_data->empty()) {
                ignore_data->erase(ignore_data->begin() + selected_ignore_filter);
                config::discord::alert_rules::ignore_filter.save();
                selected_ignore_filter = max(0, selected_ignore_filter - 1);
            }
            ImGui::SameLine();
            if (ImGui::Button(" + ##ignore_filter")) { new_ignore_popup = true; }

            if (new_ignore_popup) {
                ImGui::OpenPopup("Add an ignore filter");
                if (ImGui::BeginPopupModal("Add an ignore filter",
                                           &new_ignore_popup,
                                           ImGuiWindowFlags_AlwaysAutoResize)) {
                    static char name_buffer[256] = {};
                    if (ImGui::IsWindowAppearing()) {
                        memset(name_buffer, 0, sizeof(name_buffer));
                    }

                    ImGui::InputText("##input_ignore_filter", name_buffer,
                                     IM_ARRAYSIZE(name_buffer));
                    if (ImGui::Button("OK")) {
                        new_ignore_popup = false;
                        ignore_data->push_back(_strdup(name_buffer));
                        config::discord::alert_rules::ignore_filter.save();
                        selected_ignore_filter = ignore_data->size() - 1;
                    }
                    ImGui::EndPopup();
                }
            }

            ImGui::SetCursorPos({10, 107});
            ImGui::Text("@everyone filter:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.31f);
            ImGui::SetCursorPos({150, 104});
            auto* ping_data = config::discord::alert_rules::ping_filter.get_ptr();
            ImGui::Combo("##ping_filter", &selected_ping_filter, ping_data->data(),
                         ping_data->size());

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "An alert message containing any of these terms will @everyone instead of @alert");
            }

            ImGui::SameLine();
            if (ImGui::Button(" - ##ping_filter") && !ping_data->empty()) {
                ping_data->erase(ping_data->begin() + selected_ping_filter);
                config::discord::alert_rules::ping_filter.save();
                selected_ping_filter = max(0, selected_ping_filter - 1);
            }
            ImGui::SameLine();
            if (ImGui::Button(" + ##ping_filter")) { new_ping_popup = true; }

            if (new_ping_popup) {
                ImGui::OpenPopup("Add a ping filter");
                if (ImGui::BeginPopupModal("Add a ping filter",
                                           &new_ping_popup,
                                           ImGuiWindowFlags_AlwaysAutoResize)) {
                    static char name_buffer[256] = {};
                    if (ImGui::IsWindowAppearing()) {
                        memset(name_buffer, 0, sizeof(name_buffer));
                    }

                    ImGui::InputText("##input_ping_filter", name_buffer,
                                     IM_ARRAYSIZE(name_buffer));
                    if (ImGui::Button("OK")) {
                        new_ping_popup = false;
                        ping_data->push_back(_strdup(name_buffer));
                        config::discord::alert_rules::ping_filter.save();
                        selected_ping_filter = ping_data->size() - 1;
                    }
                    ImGui::EndPopup();
                }
            }
        }
        end_child();
        ImGui::SameLine();
        begin_child("Advanced", ImVec2(280, ImGui::GetWindowHeight()));
        {
            ImGui::SetCursorPos({10, 11});
            if (ImGui::Checkbox("Flush tribelogs",
                                config::discord::advanced::flush_logs.get_ptr())) {
                config::discord::advanced::flush_logs.save();
                                }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("Enable to keep the previous 50 events in the log channel only.");
            }
        }
        end_child();

        ImGui::SetCursorPosY((ImGui::GetWindowHeight() / 2) + 5);
        begin_child("Roles & Channels",
                    ImVec2(475 - maintabs_data.width, ImGui::GetWindowHeight() / 2));
        {

            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Logs channel:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 11});
            if (ImGui::InputText("##log_channel",
                                 config::discord::channels::logs.get_ptr())) {
                config::discord::channels::logs.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The channel to post regular logs to.");
            }
            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Alert channel:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 42});
            if (ImGui::InputText("##alert_channel",
                                 config::discord::channels::alert.get_ptr())) {
                config::discord::channels::alert.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "The channel to post alerts to. If empty logs channel is used.");
            }

            ImGui::SetCursorPos({10, 76});
            ImGui::Text("Alert role:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({150, 73});
            if (ImGui::InputText("##alert_role",
                                 config::discord::roles::alert.get_ptr())) {
                config::discord::roles::alert.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "The role to be mentioned for parasaur or tribelog alerts.\n"
                        "If empty, @everyone will always be used.");
            }
        }
        end_child();
    }


    void draw_bots_paste_tabs()
    {
        begin_child("Station Configuration",
                    ImVec2(475 - maintabs_data.width, ImGui::GetWindowHeight() * 0.52));
        {
            ImGui::SetCursorPos({10, 5});
            ImGui::Text("Station prefix:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 2});
            if (ImGui::InputText("##paste_prefix",
                                 config::bots::paste::prefix.get_ptr())) {
                config::bots::paste::prefix.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "Specifies the prefix for your paste beds. The prefix must "
                        "be included in your bed name but may not be identical.\n"
                        "For instance, your bed could be named COOL SPOT // "
                        "PASTE00, while your prefix can still be PASTE.");
            }

            ImGui::SetCursorPos({10, 36});
            ImGui::Text("Render prefix:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 33});
            if (ImGui::InputText("##render_prefix",
                                 config::bots::paste::render_prefix.get_ptr())) {
                config::bots::paste::render_prefix.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "Specifies the prefix for the render bed. For more "
                        "details, please refer to the explanation below.\n\n"
                        "Why is a render bed needed?\n"
                        "When the snail is loaded before the structure it's on, it "
                        "visually glitches into it.\n"
                        "To resolve this issue, you need to render the structure, "
                        "leave the render of the dino, and then move back to it.\n"
                        "To achieve this, you'll require a source bed close to the "
                        "snails named [PREFIX]::SRC\n"
                        "and a gateway bed out of snail render named " "[PREFIX]::GATEWAY.");
            }

            ImGui::SetCursorPos({10, 67});
            ImGui::Text("Station count:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 64});
            if (ImGui::InputInt("##paste_count",
                                config::bots::paste::num_stations.get_ptr(), 1, 5)) {
                config::bots::paste::num_stations.save();
            }
            int* num_stations = config::bots::paste::num_stations.get_ptr();
            *num_stations = std::clamp(*num_stations, 1, 100);

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The number of paste stations you have.");
            }


            ImGui::SetCursorPos({10, 98});
            ImGui::Text("Interval (minutes):");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 95});
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


            ImGui::SetCursorPos({10, 129});
            ImGui::Text("Load for (seconds):");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 126});
            if (ImGui::InputInt("##paste_render", config::bots::paste::load_for.get_ptr(),
                                1, 5)) { config::bots::paste::load_for.save(); }
            int* load_for = config::bots::paste::load_for.get_ptr();
            *load_for = std::clamp(*load_for, 5, 100);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "Specifies the duration for stations to render at the "
                        "source render bed (in seconds).\n"
                        "The rendering time increases with more structures; "
                        "additional structures require a longer rendering time.");
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
                        "Even when disabled, the paste manager is created but "
                        "remains inactive.\n"
                        "You can toggle its state at runtime or through the " "discord bot.");
            }

            if (ImGui::Checkbox("Allow partial completion",
                                config::bots::paste::allow_partial.get_ptr())) {
                config::bots::paste::allow_partial.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "Enables the bot to pause its operation after reaching a "
                        "specific station and resume afterward.\n\n"
                        "For example:\nSuppose there are Task A, Task B, and Task "
                        "C in order of priority. Task C represents\n"
                        "a PasteManager instance handling paste stations. If both "
                        "Task A and B are on cooldown and C is initiated,\n"
                        "A and B will be temporarily ignored until C completes. In "
                        "cases of partial completion, C might finish\n"
                        "station 3, execute Task A/B, and then resume C at station " "4.\n\n"
                        "Not recommended if there will be frequent partial "
                        "completions and rendering takes a considerable amount of "
                        "time.\n");
            }


            if (ImGui::Checkbox("OCR deposited amount",
                                config::bots::paste::ocr_amount.get_ptr())) {
                config::bots::paste::ocr_amount.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "Enable to use OCR to determine how much paste was put "
                        "into the dedi per station.\n"
                        "This info is sent to the completion embed on discord, it "
                        "has no other purpose as of now.");
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
        begin_child("Crate Managers",
                    ImVec2(425 - maintabs_data.width, ImGui::GetWindowHeight() * 0.33));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Selected Manager:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 11});
            std::vector<const char*>* data = config::bots::drops::managers.get_ptr();
            ImGui::Combo("##selected_manager", &selected_manager, data->data(),
                         data->size());

            ImGui::SetCursorPos({10, 45});
            if (ImGui::Button("Add new")) { new_name_popup = true; }
            ImGui::SameLine();
            if (ImGui::Button("Delete selected") && !data->empty()) {
                confirmation_popup = true;
            }
            ImGui::SameLine();

            if (ImGui::Button("Rename selected") && !data->empty()) {
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
                        selected_manager = data->size() - 1;
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
                        auto map = config::bots::drops::configs;
                        map[name].erase();
                        map.erase(name);
                        config::bots::drops::managers.save();
                        selected_manager = max(0, selected_manager - 1);
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

        // get the crate manager instance thats selected, if none is selected
        // then make everything inactive
        config::ManagedVar<bots::drops::CrateManagerConfig>* active = nullptr;

        if (!config::bots::drops::managers.get_ptr()->empty()) {
            const std::string selected = (*config::bots::drops::managers.get_ptr())[
                    selected_manager];
            if (config::bots::drops::configs.contains(selected)) {
                active = &config::bots::drops::configs[selected];
            } else {
                config::bots::drops::configs[selected] = config::ManagedVar(
                        {"bots", "drops", selected}, config::save,
                        bots::drops::CrateManagerConfig());
            }
        }

        ImGui::SameLine();
        begin_child("Advanced", ImVec2(270, ImGui::GetWindowHeight() * 0.53));
        {
            if (active) {
                ImGui::SetCursorPos({10, 11});
                if (ImGui::Checkbox("Allow partial completion",
                                    &active->get_ptr()->allow_partial_completion)) {
                    active->save();
                }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                    ImGui::SetTooltip(
                            "[ONLY SUPPORTED FOR BED STATIONS]\n\n"
                            "Allow the bot to break after a certain station and "
                            "pick back up after that.\n\n"
                            "Example:\nConsider we have Task A, Task B and Task C "
                            "where the order is their priority and C is\n"
                            "our CrateManager instance managing the crates.\nIf "
                            "Task A and B are both on cooldown and C is started, A "
                            "and B will be disregarded\n"
                            "until C completes. With partial completion, C may go "
                            "to complete station 3, complete task A/B,\n"
                            "then pick C back up at 4.");
                }

                ImGui::SetCursorPos({10, 42});
                ImGui::Text("Load align (sec):");
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
                ImGui::SetCursorPos({130, 39});
                if (ImGui::InputInt("##crate_render_align",
                                    &active->get_ptr()->render_align_for, 1, 5)) {
                    active->save();
                }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                    ImGui::SetTooltip("How long to let the align station render for.");
                }

                ImGui::SetCursorPos({10, 73});
                ImGui::Text("Load start (sec):");
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
                ImGui::SetCursorPos({130, 70});
                if (ImGui::InputInt("##crate_render_start",
                                    &active->get_ptr()->render_initial_for, 1, 5)) {
                    active->save();
                }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                    ImGui::SetTooltip(
                            "How long to render the first drop of the entire manager for.");
                }

                ImGui::SetCursorPos({10, 104});
                ImGui::Text("Load groups (sec):");
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
                ImGui::SetCursorPos({130, 101});
                if (ImGui::InputInt("##crate_render_group",
                                    &active->get_ptr()->render_group_for, 1, 5)) {
                    active->save();
                }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                    ImGui::SetTooltip(
                            "How long to render the first drop of each group for.");
                }
            }

            ImGui::SetCursorPos({10, active ? 135.f : 14.f});
            ImGui::Text("Vault limit (%%):");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
            ImGui::SetCursorPos({130, active ? 132.f : 11.f});
            if (ImGui::InputInt("##vault_threshold",
                                config::bots::drops::vault_alert_threshold.get_ptr(), 5,
                                10)) {
                config::bots::drops::vault_alert_threshold.save();
            }
            int* thresh = config::bots::drops::vault_alert_threshold.get_ptr();
            *thresh = std::clamp(*thresh, 20, 100);

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "[GLOBAL]\nThe maximum %% of slots the vaults the items "
                        "are dumped into is allowed to have.\n"
                        "When the threshold is exceeded, the 'helper [no access]' "
                        "role will be tagged to empty it.");
            }
        }
        end_child();

        ImGui::SetCursorPosY((ImGui::GetWindowHeight() * 0.33) + 5);
        begin_child("Configuration",
                    ImVec2(425 - maintabs_data.width, ImGui::GetWindowHeight() * 0.66));
        {
            if (active) {
                ImGui::SetCursorPos({10, 14});
                ImGui::Text("Station prefix:");
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
                ImGui::SetCursorPos({150, 11});
                if (ImGui::InputText("##crate_prefix", &active->get_ptr()->prefix)) {
                    active->save();
                }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                    ImGui::SetTooltip(
                            "The prefix of this crate manager.\n\n"
                            "The following structure instances will be created "
                            "with the prefix:\n"
                            "-[PREFIX]::ALIGN - The initial bed to spawn on for "
                            "teleporter mode\n"
                            "-[PREFIX]::DROPXX - The teleporter / bed per drop "
                            "where XX is it's index\n"
                            "-[PREFIX]::DROPOFF - The vault the items will be "
                            "stored in for teleporter mode\n");
                }

                ImGui::SetCursorPos({10, 45});
                ImGui::Text("Grouped Crates:");
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
                ImGui::SetCursorPos({150, 42});

                if (ImGui::InputText("##crate_groups",
                                     &active->get_ptr()->grouped_crates_raw)) {
                    active->save();
                }

                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                    ImGui::SetTooltip(
                            "An array of arrays where each array represents one "
                            "group of crates.\n"
                            "Each group corresponds to crates that share the same "
                            "uptime,\n"
                            "meaning only one of them will be active at any given "
                            "time (e.g., Swamp Cave big room).\n\n" "Guidelines:\n"
                            " - Enclose each group in curly braces { and }.\n"
                            " - Separate elements within a group with commas.\n"
                            " - For each crate 'j' per crates[i], provide one bed "
                            "or tp with the corresponding name.\n"
                            " - Define each crate with its color options: BLUE, "
                            "YELLOW, RED.\n"
                            " - Use a pipe '|' to separate multiple options for a "
                            "crate (e.g., YELLOW | RED).\n"
                            " - 'ANY' can be used to represent multiple color "
                            "options for a crate.\n\n" "Example for Island Swamp Cave:\n"
                            "{RED, RED}, {YELLOW, YELLOW, ANY}, {BLUE}");
                }

                ImGui::SetCursorPos({10, 76});
                ImGui::Text("Interval (minutes):");
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
                ImGui::SetCursorPos({150, 73});
                if (ImGui::InputInt("##crate_interval", &active->get_ptr()->interval, 1,
                                    5)) { active->save(); }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                    ImGui::SetTooltip(
                            "The interval to complete the stations " "at (in minutes).");
                }

                ImGui::SetCursorPos({10, 107});
                if (ImGui::Checkbox("Uses teleporters",
                                    &active->get_ptr()->uses_teleporters)) {
                    active->save();
                }

                ImGui::SameLine();
                if (ImGui::Checkbox("Disabled##crate_manager2",
                                    &active->get_ptr()->disabled)) { active->save(); }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                    ImGui::SetTooltip(
                            "Completely disable this crate manager's " "completion.\n\n"
                            "Even when disabled, the paste manager "
                            "is created but remains inactive.\n"
                            "You can toggle its state at runtime or "
                            "through the discord bot.");
                }
            }
        }
        end_child();
        ImGui::SameLine();
        ImGui::SetCursorPosY((ImGui::GetWindowHeight() * 0.53) + 5);
        begin_child("Rerolling", ImVec2(270, ImGui::GetWindowHeight() * 0.47));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Loot Channel:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.55f);
            ImGui::SetCursorPos({100, 11});
            if (ImGui::InputText("##loot_channel",
                                 config::bots::drops::loot_channel.get_ptr())) {
                config::bots::drops::loot_channel.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "The channel to post the crate loot to, leave empty to post to info channel.");
            }

            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Reroll Role:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.55f);
            ImGui::SetCursorPos({100, 42});
            if (ImGui::InputText("##reroll_role",
                                 config::bots::drops::reroll_role.get_ptr())) {
                config::bots::drops::reroll_role.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "The role to mention when a drop is ready to be rerolled.\n"
                        "Leave empty to not mention any role.");
            }

            ImGui::SetCursorPos({10, 73});
            if (ImGui::Checkbox("Reroll Mode Enabled",
                                config::bots::drops::reroll_mode.get_ptr())) {
                config::bots::drops::reroll_mode.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "[GLOBAL]\nWhen enabled, Ling Ling++ will "
                        "request a reroll of a found crate.");
            }
            if (active) {
                ImGui::SetCursorPos({10, 104});
                if (ImGui::Checkbox("Overrule reroll mode",
                                    &active->get_ptr()->overrule_reroll_mode)) {
                    active->save();
                }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                    ImGui::SetTooltip(
                            "Enable to ignore the reroll mode for this crate "
                            "manager, drops are always looted.");
                }
            }
        }
        end_child();
    }

    void draw_bots_crops_tabs()
    {
        begin_child("Station Configuration",
                    ImVec2(475 - maintabs_data.width, ImGui::GetWindowHeight()));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Longrass stations:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 11});
            if (ImGui::InputInt("##num_longrass",
                                config::bots::crops::num_longrass.get_ptr(), 1, 5)) {
                config::bots::crops::num_longrass.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The number of longrass stations you have.");
            }

            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Citronal stations:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 42});
            if (ImGui::InputInt("##num_citronal",
                                config::bots::crops::num_citronal.get_ptr(), 1, 5)) {
                config::bots::crops::num_citronal.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The number of citronal stations you have.");
            }

            ImGui::SetCursorPos({10, 76});
            ImGui::Text("Rockarrot stations:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 73});
            if (ImGui::InputInt("##num_rockarrot",
                                config::bots::crops::num_rockarrot.get_ptr(), 1, 5)) {
                config::bots::crops::num_rockarrot.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The number of rockarrot stations you have.");
            }

            ImGui::SetCursorPos({10, 107});
            ImGui::Text("Savoroot stations:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 104});
            if (ImGui::InputInt("##num_savoroot",
                                config::bots::crops::num_savoroot.get_ptr(), 1, 5)) {
                config::bots::crops::num_savoroot.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The number of savoroot stations you have.");
            }


            ImGui::SetCursorPos({10, 138});
            ImGui::Text("Interval (hours):");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 135});
            if (ImGui::InputInt("##crop_interval",
                                config::bots::crops::interval.get_ptr(), 1, 24)) {
                config::bots::crops::interval.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "The interval to complete the station at (in hours).\n"
                        "150 crops take 8 hours to grow on 300%% greenhouse " "effect.");
            }
        }
        end_child();

        ImGui::SameLine();
        begin_child("Advanced", ImVec2(280, ImGui::GetWindowHeight()));
        {
            ImGui::SetCursorPos({10, 11});
            if (ImGui::Checkbox("Disable station completion",
                                config::bots::crops::disabled.get_ptr())) {
                config::bots::crops::disabled.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "Completely disable the crop manager's completion.\n\n"
                        "Even when disabled, crop sap manager is created but "
                        "remains inactive.\n"
                        "You can toggle its state at runtime or through the " "discord bot.");
            }
            end_child();
        }
    }

    void draw_bots_sap_tabs()
    {
        begin_child("Station Configuration",
                    ImVec2(475 - maintabs_data.width, ImGui::GetWindowHeight()));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Station prefix:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 11});
            if (ImGui::InputText("##sap_prefix", config::bots::sap::prefix.get_ptr())) {
                config::bots::sap::prefix.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "Specifies the prefix for your sap beds. The prefix must "
                        "be included in your bed name but may not be identical.\n"
                        "For instance, your bed could be named COOL MYSAP01, while "
                        "your prefix can still be SAP.");
            }

            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Station count:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 42});
            if (ImGui::InputInt("##sap_count", config::bots::sap::num_stations.get_ptr(),
                                1, 5)) { config::bots::sap::num_stations.save(); }
            int* num_stations = config::bots::sap::num_stations.get_ptr();
            *num_stations = std::clamp(*num_stations, 1, 100);

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The number of sap stations you have.");
            }

            ImGui::SetCursorPos({10, 76});
            ImGui::Text("Interval (minutes):");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({150, 73});
            if (ImGui::InputInt("##sap_interval", config::bots::sap::interval.get_ptr(),
                                1, 5)) { config::bots::sap::interval.save(); }
            int* interval = config::bots::sap::interval.get_ptr();
            *interval = std::clamp(*interval, 5, 150);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "The interval to complete the station at (in minutes).");
            }
        }
        end_child();

        ImGui::SameLine();
        begin_child("Advanced", ImVec2(280, ImGui::GetWindowHeight()));
        {
            ImGui::SetCursorPos({10, 11});
            if (ImGui::Checkbox("Disable station completion",
                                config::bots::sap::disabled.get_ptr())) {
                config::bots::sap::disabled.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "Completely disable the sap manager's completion.\n\n"
                        "Even when disabled, the sap manager is created but "
                        "remains inactive.\n"
                        "You can toggle its state at runtime or through the " "discord bot.");
            }
            end_child();
        }
    }

    void draw_bots_crafting_tabs()
    {
        begin_child("Sparkpowder",
                    ImVec2((ImGui::GetWindowWidth() * 0.42),
                           ImGui::GetWindowHeight() * 0.47));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Prefix:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({120, 11});
            if (ImGui::InputText("##spark_prefix",
                                 config::bots::crafting::spark::prefix.get_ptr())) {
                config::bots::crafting::spark::prefix.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "Specifies the prefix for your spark beds. The prefix must "
                        "be included in your bed name but may not be identical.\n"
                        "For instance, your bed could be named HELLO SPARK01, while "
                        "your prefix can still be SPARK.");
            }

            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Stations:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({120, 42});
            if (ImGui::InputInt("##spark_count",
                                config::bots::crafting::spark::num_stations.get_ptr(), 1,
                                5)) {
                config::bots::crafting::spark::num_stations.save();
            }
            int* num_stations = config::bots::crafting::spark::num_stations.get_ptr();
            *num_stations = std::clamp(*num_stations, 1, 100);

            spark_tooltip = std::vformat(
                    "The number of sparkpowder stations you have.\n"
                    "Current processing power:\n"
                    "{} sparkpowder every 6 minutes using {} flint & {} stone",
                    std::make_format_args(*num_stations * 9600, *num_stations * 6400,
                                          *num_stations * 3200));

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(spark_tooltip.c_str());
            }

            ImGui::SetCursorPos({10, 76});
            ImGui::Text("Interval (m):");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({120, 73});
            if (ImGui::InputInt("##spark_interval",
                                config::bots::crafting::spark::interval.get_ptr(), 1,
                                5)) { config::bots::crafting::spark::interval.save(); }
            int* interval = config::bots::crafting::spark::interval.get_ptr();
            *interval = std::clamp(*interval, 5, 150);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "The interval to complete the station at (in minutes).");
            }
            ImGui::SetCursorPos({10, 107});
            if (ImGui::Checkbox("Disabled",
                                config::bots::crafting::spark::disabled.get_ptr())) {
                config::bots::crafting::spark::disabled.save();
            }
        }
        end_child();
        ImGui::SameLine();
        begin_child("Gunpowder",
                    ImVec2((ImGui::GetWindowWidth() * 0.41),
                           ImGui::GetWindowHeight() * 0.47));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Prefix:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({120, 11});
            if (ImGui::InputText("##gunpowder_prefix",
                                 config::bots::crafting::gunpowder::prefix.get_ptr())) {
                config::bots::crafting::gunpowder::prefix.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "Specifies the prefix for your gunpowder beds. The prefix must "
                        "be included in your bed name but may not be identical.\n"
                        "For instance, your bed could be named TEST GP01, while "
                        "your prefix can still be GP.");
            }

            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Stations:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({120, 42});
            if (ImGui::InputInt("##gunpowder_count",
                                config::bots::crafting::gunpowder::num_stations.get_ptr(),
                                1, 5)) {
                config::bots::crafting::gunpowder::num_stations.save();
            }
            int* num_stations = config::bots::crafting::gunpowder::num_stations.get_ptr();
            *num_stations = std::clamp(*num_stations, 1, 100);

            gp_tooltip = std::vformat(
                    "The number of gunpowder stations you have.\n"
                    "Current processing power:\n"
                    "{} gunpowder every 6 minutes using {} sparkpowder & {} charcoal",
                    std::make_format_args(*num_stations * 6000, *num_stations * 4000,
                                          *num_stations * 4000));

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(gp_tooltip.c_str());
            }

            ImGui::SetCursorPos({10, 76});
            ImGui::Text("Interval (m):");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({120, 73});
            if (ImGui::InputInt("##gunpowder_interval",
                                config::bots::crafting::gunpowder::interval.get_ptr(), 1,
                                5)) {
                config::bots::crafting::gunpowder::interval.save();
            }
            int* interval = config::bots::crafting::gunpowder::interval.get_ptr();
            *interval = std::clamp(*interval, 5, 150);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "The interval to complete the station at (in minutes).");
            }
            ImGui::SetCursorPos({10, 107});
            if (ImGui::Checkbox("Disabled",
                                config::bots::crafting::gunpowder::disabled.get_ptr())) {
                config::bots::crafting::gunpowder::disabled.save();
            }
        }
        end_child();
        begin_child("Paste Grinding",
                    ImVec2((ImGui::GetWindowWidth() * 0.42),
                           ImGui::GetWindowHeight() * 0.47));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Prefix:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({120, 11});
            if (ImGui::InputText("##grinding_prefix",
                                 config::bots::crafting::grinding::prefix.get_ptr())) {
                config::bots::crafting::grinding::prefix.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("Specifies the prefix for your grinding beds.");
            }

            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Stations:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({120, 42});
            if (ImGui::InputInt("##grinding_count",
                                config::bots::crafting::grinding::num_stations.get_ptr(),
                                1, 5)) {
                config::bots::crafting::grinding::num_stations.save();
            }
            int* num_stations = config::bots::crafting::grinding::num_stations.get_ptr();
            *num_stations = std::clamp(*num_stations, 1, 100);

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The number of grinding stations you have.");
            }

            ImGui::SetCursorPos({10, 76});
            ImGui::Text("Interval (m):");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({120, 73});
            if (ImGui::InputInt("##grinding_interval",
                                config::bots::crafting::grinding::interval.get_ptr(), 1,
                                5)) {
                config::bots::crafting::grinding::interval.save();
            }
            int* interval = config::bots::crafting::grinding::interval.get_ptr();
            *interval = std::clamp(*interval, 5, 150);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "The interval to complete the station at (in minutes).");
            }
            ImGui::SetCursorPos({10, 107});
            if (ImGui::Checkbox("Disabled",
                                config::bots::crafting::grinding::disabled.get_ptr())) {
                config::bots::crafting::grinding::disabled.save();
            }
        }
        end_child();
        ImGui::SameLine();
        begin_child("Advanced Rifle Bullets",
                    ImVec2((ImGui::GetWindowWidth() * 0.41),
                           ImGui::GetWindowHeight() * 0.47));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Prefix:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({120, 11});
            if (ImGui::InputText("##bullets_prefix",
                                 config::bots::crafting::arb::prefix.get_ptr())) {
                config::bots::crafting::arb::prefix.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("Specifies the prefix for your arb beds");
            }

            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Stations:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({120, 42});
            if (ImGui::InputInt("##bullets_count",
                                config::bots::crafting::arb::num_stations.get_ptr(),
                                1, 5)) {
                config::bots::crafting::arb::num_stations.save();
            }
            int* num_stations = config::bots::crafting::arb::num_stations.get_ptr();
            *num_stations = std::clamp(*num_stations, 1, 100);

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The number of arb stations you have");
            }

            ImGui::SetCursorPos({10, 76});
            ImGui::Text("Interval (m):");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({120, 73});
            if (ImGui::InputInt("##bullets_interval",
                                config::bots::crafting::arb::interval.get_ptr(), 1,
                                5)) {
                config::bots::crafting::arb::interval.save();
            }
            int* interval = config::bots::crafting::arb::interval.get_ptr();
            *interval = std::clamp(*interval, 5, 150);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "The interval to complete the station at (in minutes).");
            }
            ImGui::SetCursorPos({10, 107});
            if (ImGui::Checkbox("Disabled",
                                config::bots::crafting::arb::disabled.get_ptr())) {
                config::bots::crafting::arb::disabled.save();
            }
        }
        end_child();
        begin_child("Forges",
                    ImVec2((ImGui::GetWindowWidth() * 0.42),
                           ImGui::GetWindowHeight() * 0.47));
        {
            ImGui::SetCursorPos({10, 14});
            ImGui::Text("Prefix:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({120, 11});
            if (ImGui::InputText("##forges_prefix",
                                 config::bots::crafting::forges::prefix.get_ptr())) {
                config::bots::crafting::forges::prefix.save();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("Specifies the prefix for your forges beds.");
            }

            ImGui::SetCursorPos({10, 45});
            ImGui::Text("Stations:");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({120, 42});
            if (ImGui::InputInt("##forges_count",
                                config::bots::crafting::forges::num_stations.get_ptr(),
                                1, 5)) {
                config::bots::crafting::forges::num_stations.save();
            }
            int* num_stations = config::bots::crafting::forges::num_stations.get_ptr();
            *num_stations = std::clamp(*num_stations, 1, 100);

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("The number of forges stations you have.");
            }

            ImGui::SetCursorPos({10, 76});
            ImGui::Text("Interval (m):");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SetCursorPos({120, 73});
            if (ImGui::InputInt("##forges_interval",
                                config::bots::crafting::forges::interval.get_ptr(), 1,
                                5)) {
                config::bots::crafting::forges::interval.save();
            }
            int* interval = config::bots::crafting::forges::interval.get_ptr();
            *interval = std::clamp(*interval, 5, 150);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip(
                        "The interval to complete the station at (in minutes).");
            }
            ImGui::SetCursorPos({10, 107});
            if (ImGui::Checkbox("Disabled",
                                config::bots::crafting::forges::disabled.get_ptr())) {
                config::bots::crafting::forges::disabled.save();
            }
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

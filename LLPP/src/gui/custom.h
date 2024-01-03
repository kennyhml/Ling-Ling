#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include <Windows.h>
#include <string>
#include <shobjidl.h>
#include <functional>
#include <string>
#include "../../external/imgui/imgui.h"
#include "../fonts/IconsFontAwesome6.h"


namespace llpp::gui
{
    inline bool openFolder(std::string& folder_path_out)
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

    enum MainTabs : int
    {
        GENERAL,
        BOTS,
        DISCORD,
        PROFILE,
        MISC,
        LOGGING,
    };

    enum GeneralTabs : int
    {
        ARK,
        BOT,
    };

    enum DiscordTabs : int
    {
        BOT_CONFIG,
        INFO,
        ALERTS,
    };

    enum BotTabs : int
    {
        PASTE,
        DROPS,
        CROPS,
        SAP,
        BREWS,
    };

    struct TabAreaData
    {
        bool is_hovered;
        bool expand;
        float width;
    };

    inline TabAreaData maintabs_data = {false, true, 0.f};

    inline float animation = 0.f;
    inline MainTabs selected_main_tab = GENERAL;

    inline std::vector<const char*> main_tabs = {
        "General", "Bots", "Discord", "Profile", "Misc", "Logging"
    };

    inline GeneralTabs selected_general_tab = ARK;
    inline std::vector<const char*> general_subtabs{"Ark", "Bot"};

    inline DiscordTabs selected_discord_tab = BOT_CONFIG;
    inline std::vector<const char*> discord_subtabs{"Bot Config", "Info", "Alerts"};

    inline BotTabs selected_bot_tab = PASTE;
    inline std::vector<const char*> bot_subtabs{
        "Paste", "Drops", "Crops", "Sap", "Brews"
    };

    inline std::vector<const char*> main_tab_icons = {
        ICON_FA_GEAR, ICON_FA_ROBOT, ICON_FA_BELL, ICON_FA_PERSON_BREASTFEEDING,
        ICON_FA_WIFI, ICON_FA_NOTE_STICKY
    };

    bool tab_button(const char* icon, const char* label, bool selected, float rounding,
                    ImDrawFlags flags);
    bool sub_tab_button(const char* label, bool selected);
    void render_main_tab_area(const std::string& name, ImVec2 size,
                              const std::function<void()>& render_content);

    template <typename T>
    void render_subtab_buttons(const std::vector<const char*>& buttons, T& selected)
    {
        ImGui::SetCursorPos(ImVec2(maintabs_data.width + 15, 33));
        ImGui::BeginGroup();
        for (int i = 0; i < buttons.size(); i++) {
            const bool is_selected = (selected == i);
            if (sub_tab_button(buttons[i], is_selected) && !is_selected) {
                selected = static_cast<T>(i);
                animation = 0.f;
            }
            if (i != buttons.size() - 1) { ImGui::SameLine(); }
        }
        ImGui::EndGroup();
    }

    void draw_general_ark_tabs();
    void draw_general_bot_tabs();


    void draw_discord_bot_config();
    void draw_discord_info_tabs();

    void draw_bots_paste_tabs();
    void draw_bots_drops_tab();

    void begin_child(const char* name, ImVec2 size);
    void end_child();
}

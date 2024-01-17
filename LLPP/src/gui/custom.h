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
        CRAFTING,
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
        "Paste", "Crates", "Crops", "Sap", "Crafting", "Brews"
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
    void draw_bots_crops_tabs();
    void draw_bots_sap_tabs();
    void draw_bots_crafting_tabs();


    void begin_child(const char* name, ImVec2 size);
    void end_child();
}

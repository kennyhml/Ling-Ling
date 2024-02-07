#pragma once
#include "tabs.h"

namespace llpp::gui::state
{
    struct TabAreaData
    {
        bool is_hovered;
        bool expand;
        float width;
    };

    inline TabAreaData maintabs_data = {false, true, 0.f};

    inline float animation = 0.f;

    inline MainTabs selected_main_tab = MainTabs::GENERAL;

    inline GeneralTabs selected_general_tab = GeneralTabs::ARK;

    inline DiscordTabs selected_discord_tab = DiscordTabs::BOT_CONFIG;

    inline BotTabs selected_bot_tab = BotTabs::PASTE;







}
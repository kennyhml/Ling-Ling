#pragma once
#include <vector>
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

    inline const std::vector MAIN_TAB_NAMES = {
        "General", "Bots", "Discord", "Profile", "Misc", "Logging"
    };

    inline std::vector MAIN_TAB_ICONS = {
        ICON_FA_GEAR, ICON_FA_ROBOT, ICON_FA_BELL, ICON_FA_PERSON_BREASTFEEDING,
        ICON_FA_WIFI, ICON_FA_NOTE_STICKY
    };

    enum GeneralTabs : int
    {
        ARK,
        BOT,
    };

    inline const std::vector GENERAL_TAB_NAMES{"Ark", "Bot"};


    enum DiscordTabs : int
    {
        BOT_CONFIG,
        INFO,
        LOGS_AND_ALERTS,
    };

    inline const std::vector DISCORD_TAB_NAMES{"Bot Config", "Info", "Logs & Alerts"};

    enum BotTabs : int
    {
        PASTE,
        DROPS,
        CROPS,
        SAP,
        CRAFTING,
        PARASAUR,
        FARM,
    };

    inline std::vector BOT_TAB_NAMES{
        "Paste", "Crates", "Crops", "Sap", "Processing", "Parasaur", "Farm"
    };

}

#pragma once
#include <string>
#include "managedvar.h"
#include <filesystem>

namespace llpp::config
{
    json& get_data();
    void save();

    namespace general::ark
    {
        inline const std::vector<std::string> BASE{"general", "ark"};

        inline ManagedVar<std::filesystem::path> root_dir(
            BASE, "game_root_directory", save, "");
    }

    namespace general::bot
    {
        inline const std::vector<std::string> BASE{"general", "bot"};

        inline ManagedVar<std::string> assets(BASE, "assets", save, "assets");
    }

    namespace discord
    {
        inline const std::vector<std::string> BASE{"discord"};

        inline ManagedVar<std::string> token(BASE, "token", save, {});

        namespace authorization
        {
            inline const std::vector<std::string> BASE{"discord", "authorization"};

            inline ManagedVar<std::vector<const char*>> roles(
                BASE, "authorized_roles", save, {});
            inline ManagedVar<std::vector<const char*>> channels(
                BASE, "command_channels", save, {});
            inline ManagedVar<std::vector<const char*>> users(
                BASE, "authorized_users", save, {});
        }
    }

    namespace bots::drops
    {
        inline const std::vector<std::string> BASE{"bots", "drops"};

        inline ManagedVar<std::vector<const char*>> managers(BASE, "managers", save, {});
    }
}

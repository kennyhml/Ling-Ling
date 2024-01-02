#pragma once
#include <string>
#include "managedvar.h"
#include <filesystem>
#include "../bots/drops/parameters.h"

namespace llpp::config
{
    json& get_data();
    void save();

    namespace general::ark
    {
        inline const std::vector<std::string> BASE{"general", "ark"};

        inline ManagedVar<std::string> root_dir(BASE, "game_root_directory", save, "");
        inline ManagedVar<std::string> server(BASE, "server", save, "");
    }

    namespace general::bot
    {
        inline const std::vector<std::string> BASE{"general", "bot"};

        inline ManagedVar<std::string> assets_dir(BASE, "assets", save, "assets");
        inline ManagedVar<std::string> itemdata(BASE, "itemdata", save, "itemdata.json");
        inline ManagedVar<std::string> tessdata_dir(BASE, "tessdata", save, "tessdata");
    }

    namespace discord
    {
        inline const std::vector<std::string> BASE{"discord"};

        inline ManagedVar<std::string> token(BASE, "token", save, {});
        inline ManagedVar<std::string> guild(BASE, "guild", save, {});

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

        namespace advanced
        {
            inline const std::vector<std::string> BASE{"discord", "advanced"};

            inline ManagedVar<bool> ephemeral_replies(BASE, "ephemeral_replies", save,
                                                      false);
        }

        namespace roles
        {
            inline const std::vector<std::string> BASE{"discord", "roles"};
            using role = ManagedVar<std::string>;

            inline role helper_access(BASE, "helper_access", save, "");
            inline role helper_no_access(BASE, "helper_no_access", save, "");
        }

        namespace channels
        {
            inline const std::vector<std::string> BASE{"discord", "channels"};
            using channel = ManagedVar<std::string>;

            inline channel info(BASE, "info", save, "");
            inline channel error(BASE, "error", save, "");
        }
    }

    namespace bots::drops
    {
        inline const std::vector<std::string> BASE{"bots", "drops"};

        inline ManagedVar<std::vector<const char*>> managers(BASE, "managers", save, {});
        inline std::unordered_map<std::string, ManagedVar<
                                      llpp::bots::drops::CrateManagerConfig>> configs;
    }

    namespace bots::paste
    {
        inline const std::vector<std::string> BASE{"bots", "paste"};

        inline ManagedVar<std::string> prefix(BASE, "prefix", save, "PASTE");
        inline ManagedVar<std::string> render_prefix(BASE, "render_prefix", save,
                                                     "PASTE::RENDER");

        inline ManagedVar<int> num_stations(BASE, "num_stations", save, 16);
        inline ManagedVar<int> interval(BASE, "interval", save, 5);
        inline ManagedVar<int> load_for(BASE, "load_for", save, 15);

        inline ManagedVar<bool> disable_completion(BASE, "disabled", save, false);
        inline ManagedVar<bool> ocr_amount(BASE, "ocr_amount", save, false);
        inline ManagedVar<bool> allow_partial(BASE, "allow_partial", save, false);
    }
}

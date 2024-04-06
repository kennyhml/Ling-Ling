#pragma once

#include <string>
#include "managedvar.h"
#include <filesystem>
#include "../bots/drops/config.h"
#include "../bots/farm/common/config.h"
#include "../bots/parasaur/config.h"

namespace llpp::config
{
    nlohmann::ordered_json& get_data();

    void save();

    class BadConfigurationError final : public std::exception
    {
    public:
        explicit BadConfigurationError(std::string t_message) : message_(
            std::move(t_message)) {}

        const char* what() const override { return message_.c_str(); }

    private:
        std::string message_;
    };

    namespace user
    {
        inline ManagedVar<std::string> name({"user"}, "name", save, "");
        inline ManagedVar<std::string> key({"user"}, "key", save, "");
    }

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
        inline ManagedVar<std::string> commands(BASE, "commands", save,
                                                "gamma | debugstructures");
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

            inline ManagedVar<bool> flush_logs(BASE, "flush_logs", save, false);
        }

        namespace roles
        {
            inline const std::vector<std::string> BASE{"discord", "roles"};
            using role = ManagedVar<std::string>;

            inline role helper_access(BASE, "helper_access", save, "");
            inline role helper_no_access(BASE, "helper_no_access", save, "");

            inline role alert(BASE, "alert", save, "");
        }

        namespace channels
        {
            inline const std::vector<std::string> BASE{"discord", "channels"};
            using channel = ManagedVar<std::string>;

            inline channel info(BASE, "info", save, "");
            inline channel error(BASE, "error", save, "");
            inline channel alert(BASE, "alert", save, "");
            inline channel logs(BASE, "logs", save, "");
        }

        namespace alert_rules
        {
            inline const std::vector<std::string> BASE{"discord", "alert_rules"};

            using filter = ManagedVar<std::vector<const char*>>;

            inline ManagedVar<int> ping_cooldown(BASE, "ping_cooldown", save, 60);
            inline ManagedVar<int> ping_min_events(BASE, "ping_min_events", save, 1);

            inline filter ignore_filter(BASE, "ignore_filter", save, {"Baby", "Decay"});
            inline filter ping_filter(BASE, "ping_filter", save, {"Pin Coded"});
        }
    }

    namespace bots::drops
    {
        inline const std::vector<std::string> BASE{"bots", "drops"};

        inline ManagedVar<std::vector<const char*>> managers(BASE, "managers", save, {});
        inline ManagedVar<bool> reroll_mode(BASE, "reroll_mode", save, false);
        inline ManagedVar<int> vault_alert_threshold(BASE, "vault_alert_threshold", save,
                                                     70);
        inline std::unordered_map<std::string, ManagedVar<
                                      llpp::bots::drops::CrateManagerConfig>> configs;

        inline ManagedVar<std::string> loot_channel(BASE, "loot_channel", save, "");
        inline ManagedVar<std::string> reroll_role(BASE, "reroll_role", save, "");
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

        inline ManagedVar<bool> disabled(BASE, "disabled", save, false);
        inline ManagedVar<bool> ocr_amount(BASE, "ocr_amount", save, false);
        inline ManagedVar<bool> allow_partial(BASE, "allow_partial", save, false);
        inline ManagedVar<bool> mute_pings(BASE, "mute_pings", save, false);

        inline ManagedVar<std::vector<int64_t>> times(BASE, "completion_times", save, {});
    }

    namespace bots::sap
    {
        inline const std::vector<std::string> BASE{"bots", "sap"};

        inline ManagedVar<std::vector<int64_t>> times(BASE, "completion_times", save, {});

        inline ManagedVar<std::string> prefix(BASE, "prefix", save, "SAP");
        inline ManagedVar<int> num_stations(BASE, "num_stations", save, 12);
        inline ManagedVar<int> interval(BASE, "interval", save, 5);

        inline ManagedVar<bool> disabled(BASE, "disabled", save, true);
    }

    namespace bots::crops
    {
        inline const std::vector<std::string> BASE{"bots", "crops"};

        inline ManagedVar<int> num_longrass(BASE, "num_longrass", save, 1);
        inline ManagedVar<int> num_citronal(BASE, "num_citronal", save, 1);
        inline ManagedVar<int> num_rockarrot(BASE, "num_rockarrot", save, 1);
        inline ManagedVar<int> num_savoroot(BASE, "num_savoroot", save, 1);
        inline ManagedVar<int> interval(BASE, "interval", save, 1);

        inline ManagedVar<bool> disabled(BASE, "disabled", save, true);
        inline ManagedVar<bool> dont_refert(BASE, "disabled", save, true);
    }

    namespace bots::crafting
    {
        namespace spark
        {
            inline const std::vector<std::string> BASE{"bots", "crafting", "spark"};

            inline ManagedVar<std::string> prefix(BASE, "prefix", save, "SPARK");
            inline ManagedVar<int> num_stations(BASE, "num_stations", save, 2);
            inline ManagedVar<int> interval(BASE, "interval", save, 10);
            inline ManagedVar<bool> disabled(BASE, "disabled", save, true);
        }

        namespace gunpowder
        {
            inline const std::vector<std::string> BASE{"bots", "crafting", "gunpowder"};

            inline ManagedVar<std::string> prefix(BASE, "prefix", save, "GP");
            inline ManagedVar<int> num_stations(BASE, "num_stations", save, 2);
            inline ManagedVar<int> interval(BASE, "interval", save, 10);
            inline ManagedVar<bool> disabled(BASE, "disabled", save, true);
        }

        namespace grinding
        {
            inline const std::vector<std::string> BASE{"bots", "crafting", "grinding"};

            inline ManagedVar<std::string> prefix(BASE, "prefix", save, "GRIND");
            inline ManagedVar<int> num_stations(BASE, "num_stations", save, 1);
            inline ManagedVar<int> interval(BASE, "interval", save, 5);
            inline ManagedVar<bool> disabled(BASE, "disabled", save, true);
        }

        namespace arb
        {
            inline const std::vector<std::string> BASE{"bots", "crafting", "arb"};

            inline ManagedVar<std::string> prefix(BASE, "prefix", save, "ARB");
            inline ManagedVar<int> num_stations(BASE, "num_stations", save, 1);
            inline ManagedVar<int> interval(BASE, "interval", save, 15);
            inline ManagedVar<bool> disabled(BASE, "disabled", save, true);
        }
    }

    namespace bots::parasaur
    {
        inline const std::vector<std::string> BASE{"bots", "parasaur"};

        inline ManagedVar<std::vector<const char*>> stations(BASE, "stations", save, {});

        inline ManagedVar<std::string> start_criteria(BASE, "start_criteria", save, "");
        inline ManagedVar<bool> disabled(BASE, "disabled", save, false);

        inline ManagedVar start_interval(BASE, "start_interval", save, 10);
        inline ManagedVar start_min_ready(BASE, "start_min_ready", save, 3);
        inline ManagedVar start_load(BASE, "start_load", save, 10);

        inline ManagedVar<std::string> teleport_start(BASE, "teleport_start", save,
                                                      "PARASAUR::START");

        inline std::unordered_map<std::string, ManagedVar<
                                      llpp::bots::parasaur::ParasaurConfig>> configs;
    }

    namespace bots::farm
    {
        inline const std::vector<std::string> BASE{"bots", "farm"};

        inline ManagedVar<std::vector<const char*>> managers(BASE, "managers", save, {});

        inline ManagedVar<bool> disabled(BASE, "disabled", save, false);
        inline std::unordered_map<std::string, ManagedVar<
                                      llpp::bots::farm::FarmConfig>> configs;


    }


}

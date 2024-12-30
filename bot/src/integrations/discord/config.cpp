#include "config.h"

namespace lingling::discord
{
    namespace
    {
        const json_t BASIC_CONFIG_SCHEMA = R"(
        {
            "token": "",
            "guild": "",
            "channels": {
                "category": "",
                "dashboard": "",
                "commands": ""
            }
        })"_json;
    }

    bool validate_discord_config(json_t& json)
    {
        return patch_json("Discord (Basic)", json, BASIC_CONFIG_SCHEMA);
    }

    json_t& get_channel_config()
    {
        return get_config_data().at("discord").at("channels");
    }

    json_t& get_base_config()
    {
        return get_config_data().at("discord");
    }
}

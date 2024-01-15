#pragma once

namespace llpp::config
{
    inline const char* required_data = R"(
{
    "general": {
        "ark": {
            "game_root_directory": "",
            "server": ""
        },
        "bot": {
            "assets": "assets",
            "itemdata": "itemdata.json",
            "tessdata": "tessdata"
        }
    },
    "discord": {
        "token": "",
        "authorization": {
            "command_channels": [],
            "authorized_roles": [],
            "authorized_users": []
        },
        "roles": {
            "helper_no_access": "",
            "helper_access": ""
        },
        "channels": {
            "info": "",
            "error": ""
        },
        "advanced": {
            "ephemeral_replies": false
        },
        "guild": ""
    },
    "bots": {
        "paste": {
            "prefix": "PASTE",
            "render_prefix": "RENDER",
            "num_stations": 16,
            "interval": 5,
            "load_for": 15,
            "ocr_amount": false,
            "allow_partial": false,
            "disabled": false,
            "grind_prefix": "PASTE::GRINDING",
            "num_grind_stations": 2,
            "grind_interval": 5,
            "grind_disabled": false
        },
        "drops": {
            "managers": [],
            "reroll_mode": false,
            "vault_alert_threshold": 70
        },
        "crops": {
            "num_longrass": 1,
            "num_citronal": 1,
            "num_rockarrot": 1,
            "num_savoroot": 1,
            "interval": 1,
            "disabled": true
        },
        "sap": {
            "prefix": "SAP",
            "num_stations": 12,
            "interval": 5,
            "disabled": true
        }
    }
}
)";
}

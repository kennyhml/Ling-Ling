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
            "num_stations": 20,
            "interval": 50,
            "load_for": 10,
            "ocr_amount": false,
            "allow_partial": true,
            "disabled": true,
            "grind_prefix": "PASTE::GRINDING",
            "num_grind_stations": 2,
            "grind_interval": 5,
            "grind_disabled": true
        },
        "drops": {
            "managers": [
                "SKYLORD",
                "SWAMP",
                "HUW"
            ],
            "SWAMP": {
                "prefix": "SWAMP",
                "grouped_crates_raw": "{RED, RED} {YELLOW, YELLOW, ANY} {BLUE}",
                "interval": 5,
                "render_for": 2,
                "uses_teleporters": true,
                "overrule_reroll_mode": false,
                "allow_partial_completion": false,
                "disabled": true
            },
            "SKYLORD": {
                "prefix": "SKYLORD",
                "grouped_crates_raw": "{YELLOW | RED, YELLOW | RED, YELLOW | RED}",
                "interval": 5,
                "render_for": 0,
                "uses_teleporters": true,
                "overrule_reroll_mode": false,
                "allow_partial_completion": false,
                "disabled": true
            },
            "HUW": {
                "prefix": "HUW",
                "grouped_crates_raw": "{ANY} {ANY} {ANY}",
                "interval": 5,
                "render_for": 5,
                "uses_teleporters": false,
                "overrule_reroll_mode": false,
                "allow_partial_completion": false,
                "disabled": true
            },
            "reroll_mode": false,
            "vault_alert_threshold": 75
        },
        "sap": {
            "prefix": "SAP",
            "num_stations": 12,
            "interval": 5,
            "disabled": true
        },
        "crops": {
            "num_longrass": 1,
            "num_citronal": 1,
            "num_rockarrot": 1,
            "num_savoroot": 1,
            "interval": 1,
            "disabled": true
        },
        "user": {
            "name": "",
            "key": ""
        }
    }
}
)";
}

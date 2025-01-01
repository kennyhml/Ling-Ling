#include "config.h"
#include "configuration/validate.h"

namespace lingling
{
    namespace
    {
        const json_t CONFIG_SCHEMA = R"(
        {
            "q_channel": ""
        })"_json;
    }

    bool validate_tasksystem_config(json_t& json)
    {
        return patch_json("Tasksystem", json, CONFIG_SCHEMA);
    }

    json_t& get_tasksystem_config()
    {
        return get_config_data().at("tasksystem");
    }
}

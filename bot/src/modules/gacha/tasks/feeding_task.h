#pragma once
#include "configuration/managedvar.h"

namespace lingling::gacha
{
    // Ther items a gacha can produce
    enum produce_item_type : int32_t
    {
        PRODUCE_ELEMENT_DUST,
        PRODUCE_WOOD,
        PRODUCE_CRYSTAL,
        PRODUCE_OBSIDIAN,
    };

    /**
     * @brief Configuration required from the user (through discord) for a feeding task,
     * must also be synced/maintained in the configuration file.
     */
    struct feeding_task_config
    {
        // The item the gacha should be set to produce
        produce_item_type produce;
    };
}

template<>
struct lingling::json_traits<lingling::gacha::feeding_task_config>
{
    using to_json_t = std::function<json_t(const gacha::feeding_task_config&)>;
    using from_json_t = std::function<gacha::feeding_task_config(const json_t&)>;

    to_json_t to_json = [](const gacha::feeding_task_config& value) {
        return json_t{
            {"produce", static_cast<int32_t>(value.produce)}
        };
    };

    from_json_t from_json = [](const json_t& json) {
        return gacha::feeding_task_config{
            json.at("produce").get<gacha::produce_item_type>()
        };
    };
};

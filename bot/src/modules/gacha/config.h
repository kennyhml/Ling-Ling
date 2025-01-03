#pragma once
#include "configuration/managedvar.h"
#include "tasks/feeding_task.h"

#include <string>

namespace lingling::gacha
{
    struct tower_preset
    {
        // The name of the tower, should match with the preset name.
        std::string name;

        // The naming schema for the teleporters
        std::string tp_name_schema;

        // How often the tower should run the feeding tasks, this is
        // better than having it individually on each task since it will apply to all.
        std::chrono::minutes feed_interval = std::chrono::hours(4);

        // The feeding tasks belonging to this tower, this is used to generate the gacha
        // feeding stations. One task = 2 gacha stations completed together.
        std::vector<feeding_task_config> feeding_task_configs{};
    };

    using tower_preset_ptr_t = std::shared_ptr<managed_var<tower_preset> >;

    inline std::map<std::string, tower_preset_ptr_t> tower_presets;

    bool validate_gacha_config(json_t& json);

    void add_preset(const std::string& preset);

    void delete_preset(const std::string& preset);

    void rename_preset(const std::string& preset, const std::string& new_name);

    [[nodiscard]] tower_preset_ptr_t get_preset(const std::string& preset);

    [[nodiscard]] json_t& get_preset_data(const std::optional<std::string>& preset);
}

template<>
struct lingling::json_traits<lingling::gacha::tower_preset>
{
    using to_json_t = std::function<json_t(const gacha::tower_preset&)>;
    using from_json_t = std::function<gacha::tower_preset(const json_t&)>;

    to_json_t to_json = [](const gacha::tower_preset& value) {
        // jsonify the feeding task configs.
        json_t feed_configs;
        for (const auto& data: value.feeding_task_configs) {
            feed_configs.emplace_back(
                json_traits<gacha::feeding_task_config>().to_json(data)
            );
        }

        return json_t{
            {"name", value.name}, {"tp_name_schema", value.tp_name_schema},
            {"feed_interval", value.feed_interval.count()},
            {"feeding_task_configs", feed_configs}
        };
    };

    from_json_t from_json = [](const json_t& json) {
        std::vector<gacha::feeding_task_config> feed_configs;
        for (const auto& config: json.at("feeding_task_configs")) {
            feed_configs.emplace_back(
                json_traits<gacha::feeding_task_config>().from_json(config)
            );
        }
        return gacha::tower_preset{
            json.at("name"), json.at("tp_name_schema"),
            std::chrono::minutes(json.at("feed_interval")), feed_configs
        };
    };
};

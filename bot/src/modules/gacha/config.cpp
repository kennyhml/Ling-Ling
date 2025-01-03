#include "config.h"
#include "configuration/validate.h"
#include "core/exceptions.h"

#include <asa/core/logging.h>

namespace lingling::gacha
{
    namespace
    {
        config_get_t get = []() -> json_t& { return get_preset_data(std::nullopt); };

        void assert_preset_exists(const std::string& preset, const bool should_exist)
        {
            if (tower_presets.contains(preset) == should_exist) { return; }
            if (should_exist) {
                throw lingling_error(std::format("Preset `{}` does not exist!", preset));
            }
            throw lingling_error(std::format("Preset `{}` already exists!", preset));
        }
    }

    bool validate_gacha_config(json_t& json)
    {
        // Section or presets dont exist yet, create!
        if (!json.contains("presets")) {
            json["presets"] = json_t::parse("{}");
            return true;
        }

        bool ret = false;

        // use default created objects with their json trait as schema
        const json_t tower_schema = json_traits<tower_preset>().to_json({});
        const json_t feed_schema = json_traits<feeding_task_config>().to_json({});

        for (auto& [k, v]: json.at("presets").items()) {
            ret |= patch_json(k, v, tower_schema);
            for (auto& task : v.at("feeding_task_configs")) {
                ret |= patch_json("tasks", task, feed_schema);
            }
            tower_presets[k] = std::make_shared<managed_var<tower_preset>>(k, get);
        }
        return ret;
    }

    void add_preset(const std::string& preset)
    {
        assert_preset_exists(preset, false);

        asa::get_logger()->info("Adding tower preset '{}'..", preset);
        tower_presets[preset] = std::make_shared<managed_var<tower_preset> >(preset, get);
        tower_presets.at(preset)->get_ptr()->name = preset;
        tower_presets.at(preset)->sync_to_obj();
        asa::get_logger()->info("Tower preset '{}' added..", preset);
    }

    void delete_preset(const std::string& preset)
    {
        assert_preset_exists(preset, true);

        json_t& data = get_config_data().at("gacha").at("presets");
        tower_presets.erase(preset);
        data.erase(preset);
        dump_config();
    }

    void rename_preset(const std::string& preset, const std::string& new_name)
    {
        assert_preset_exists(preset, true);
        assert_preset_exists(new_name, false);

        // Replace the data with the data of the old preset.
        json_t& data = get_preset_data(std::nullopt);
        data[new_name] = json_t(data.at(preset));
        data.at(new_name).at("name") = new_name;

        // Erase the data of the old preset
        data.erase(preset);
        tower_presets.erase(preset);

        tower_presets[new_name] = std::make_shared<managed_var<tower_preset>>(new_name, get);
        tower_presets.at(new_name)->sync_from_obj();
        tower_presets.at(new_name)->sync_to_obj();
    }

    tower_preset_ptr_t get_preset(const std::string& preset)
    {
        assert_preset_exists(preset, true);
        return tower_presets.at(preset);
    }

    json_t& get_preset_data(const std::optional<std::string>& preset)
    {
        json_t& presets = get_config_data().at("gacha").at("presets");
        try {
            if (preset) { return presets.at(*preset); }
        } catch (const json_t::out_of_range&) {
            throw lingling_error(std::format("Preset `{}` does not exist!", *preset));
        }
        return presets;

    }
}

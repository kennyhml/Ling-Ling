#include "common.h"
#include <set>

namespace lingling::discord
{
    dpp::command_option create_config_ctrl_command(
        const std::string& name,
        const std::vector<discord_configurable>& configs,
        const std::string& description,
        const bool params)
    {
        dpp::command_option command(dpp::co_sub_command, name, description);
        dpp::command_option var(dpp::co_string, "variable", "The target variable.", true);
        // Add the variable field with all the choices
        for (const auto& c: configs) {
            var.add_choice({c.variable_name, c.variable_name});
        }
        command.add_option(var);

        // Go through the required parameters and make sure every parameter type is listed
        if (params) {
            std::set<dpp::command_option_type> added_types;
            for (const auto& c: configs) {
                // Already added a parameter of this type
                if (added_types.contains(c.parameter_type)) { continue; }

                command.add_option({
                    c.parameter_type, c.parameter_name, c.parameter_description, false
                });
                added_types.emplace(c.parameter_type); // Remember that we added this type
            }
        }
        return command;
    }
}

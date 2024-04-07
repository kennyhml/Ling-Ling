#pragma once
#include <dpp/dpp.h>

namespace llpp::discord
{
    /**
     * @brief Resets all dynamic fields of the current dashboard.
     */
    void reset_dashboard();

    /**
     * @brief Updates the dashboard on discords side.
     */
    void update_dashboard();

    /**
     * @brief Registers a new field on the current dashboard message.
     *
     * The instance updating the provided field may update it as needed and the changes
     * will be reflected on the next update_dashboard call.
     *
     * @param field A shared ptr to the field to add to the dashboard.
     */
    void register_dashboard_field(std::shared_ptr<dpp::embed_field> field);
}

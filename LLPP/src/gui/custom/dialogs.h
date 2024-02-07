#pragma once
#include <string>

namespace llpp::gui::custom
{
    /**
     * @brief Wrapper for a new element popup.
     *
     * @param active Pointer to boolean determining whether the dialog is active.
     * @param text The title of the active
     * @param new_out A string reference to hold the added item.
     *
     * @return Whether the new element name should be used or not.
     */
    bool simple_add_dialog(bool* active, const char* text, std::string& new_out);

    /**
     * @brief Wrapper to confirm element deletion.
     *
     * @param active Pointer to boolean determining whether the dialog is active.
     * @param text The title of the active
     * @param confirmed Whether the deletion was confirmed or cancelled.
     *
     * @return Whether the confirmed state should be used or not.
     */
    bool simple_delete_dialog(bool* active, const char* text, bool& confirmed);
}

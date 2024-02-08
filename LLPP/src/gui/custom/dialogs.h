#pragma once
#include <string>

namespace llpp::gui::custom
{
    /**
     * @brief Wrapper for a new element popup.
     *
     * @param active Pointer to boolean determining whether the dialog is active.
     * @param text The title of the dialog window.
     * @param new_out A string reference to hold the added item.
     *
     * @return Whether the new element name should be used or not.
     */
    bool simple_add_dialog(bool* active, const char* text, std::string& new_out);

    /**
     * @brief Wrapper to confirm element deletion.
     *
     * @param active Pointer to boolean determining whether the dialog is active.
     * @param text The title of the dialog window.
     * @param confirmed Whether the deletion was confirmed or cancelled.
     *
     * @return Whether the confirmed state should be used or not.
     */
    bool simple_delete_dialog(bool* active, const char* text, bool& confirmed);

    /**
     * @brief Wrapper to confirm element renaming and get the new name.
     *
     * @param active Pointer to boolean determining whether the dialog is active.
     * @param text The title of the dialog window.
     * @param confirmed Whether the deletion was confirmed or cancelled.
     * @param new_name_out The new name chosen for the item.
     *
     * @return Whether the confirmed state should be used or not.
     */
    bool simple_rename_dialog(bool* active, const char* text, bool& confirmed,
                              std::string& new_name_out);
}

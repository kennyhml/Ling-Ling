#pragma once
#include <string>
#include <vector>
#include <dpp/dpp.h>

namespace lingling::discord
{
    enum ansi_color
    {
        ANSI_COLOR_NONE = 0,
        ANSI_COLOR_GRAY = 30,
        ANSI_COLOR_RED,
        ANSI_COLOR_GREEN,
        ANSI_COLOR_YELLOW,
        ANSI_COLOR_BLUE,
        ANSI_COLOR_PINK,
        ANSI_COLOR_CYAN,
        ANSI_COLOR_WHITE,
    };

    enum ansi_style
    {
        ANSI_STYLE_NORMAL = 0,
        ANSI_STYLE_BOLD = 1,
        ANSI_STYLE_UNDERLINE = 4
    };

    struct fmt_text
    {
        std::string content;
        int32_t box_width;

        ansi_color color = ANSI_COLOR_NONE;
        ansi_style style = ANSI_STYLE_NORMAL;
    };

    using text_row_t = std::vector<fmt_text>;

    /**
     * @brief Formates a row of text taking into consideration the text, color
     * and width of the text for the table.
     *
     * @param row The row to format.
     *
     * @return A string formatted according to the input row.
     */
    [[nodiscard]] std::string fmt_table_row(const text_row_t& row);

    /**
     * @brief Creates a seperator for the provided row, only the width is used.
     *
     * @param row The row to format, only the width is used.
     *
     * @return A string formatted according to the input row.
     */
    [[nodiscard]] std::string fmt_table_sep(const text_row_t& row);

    /**
     * @brief Creates an end for the provided row, only the width is used.
     *
     * @param row The row to format, only the width is used.
     *
     * @return A string formatted according to the input row.
     */
    [[nodiscard]] std::string fmt_table_end(const text_row_t& row);

    [[nodiscard]] std::string ansi(const std::string& text, int32_t, int32_t);

    [[nodiscard]] std::vector<dpp::message> table(const std::vector<text_row_t>& rows);
}

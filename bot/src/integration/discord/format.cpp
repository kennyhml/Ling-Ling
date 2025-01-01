#include "format.h"

namespace lingling::discord
{
    std::string ansi(const std::string& text, const int32_t color, const int32_t style)
    {
        return std::format("\u001b[{};{}m{}\u001b[0m", style, color, text);
    };

    std::string fmt_table_row(const text_row_t& row)
    {
        std::string ret;

        for (const auto& [content, width, color, style]: row) {
            // Dont add ansi codes if color and style are default, its just a
            // waste of characters (2000 max per message).
            if (color == ANSI_COLOR_NONE && style == ANSI_STYLE_NORMAL) {
                ret += std::format("│ {:<{}} ", content, width);
            } else {
                ret += std::format("│ \u001b[{};{}m{:<{}}\u001b[0m ",
                                   static_cast<int>(color), static_cast<int>(style),
                                   content, width);
            }
        }
        ret += "│\n";
        return ret;
    }

    std::string fmt_table_sep(const text_row_t& row)
    {
        std::string ret = "├";

        for (size_t i = 0; i < row.size(); ++i) {
            for (size_t j = 0; j < row[i].box_width + 2; j++) {
                ret += "━";
            }
            ret += (i < row.size() - 1 ? "┼" : "┤\n");
        }
        return ret;
    }

    std::string fmt_table_end(const text_row_t& row)
    {
        std::string ret = "└";

        for (size_t i = 0; i < row.size(); ++i) {
            for (size_t j = 0; j < row[i].box_width + 2; j++) {
                ret += "━";
            }
            ret += (i < row.size() - 1 ? "┴" : "┘\n");
        }
        return ret;
    }

    std::vector<dpp::message> table(const std::vector<text_row_t>& rows)
    {
        std::vector messages{dpp::message("```ansi\n")};

        for (size_t i = 0; i < rows.size(); i++) {
            std::string row = fmt_table_row(rows[i]);
            // Last row shouldnt have a seperator but an end line instead
            row += i == rows.size() - 1 ? fmt_table_end(rows[i]) : fmt_table_sep(rows[i]);

            const size_t msg_size = dpp::utility::utf8len(messages.back().content);
            // Messages can have max 2000 characters, so due to the "```" characters we
            // still need at the end, we cannot exceed 1997 characters. In this case
            // we wrap up the current message and continue on a new one.
            if (msg_size + dpp::utility::utf8len(row) > 1997) {
                messages.back().content += "```";
                messages.emplace_back("```ansi\n" + row);
            } else {
                messages.back().content += row;
            }
        }
        messages.back().content += "```";
        return messages;
    }
}

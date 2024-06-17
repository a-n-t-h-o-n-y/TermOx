#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include <termox/core.hpp>
#include <termox/widgets/widget.hpp>

namespace ox::widgets {

struct TextBox {
    std::string text              = "";
    bool editable                 = true;
    bool word_wrap                = true;
    std::size_t cursor_index      = 0;
    std::size_t top_display_index = 0;  // index into text

    // Index into content where each line starts.
    std::vector<std::size_t> line_start_indices = {0};
};

auto paint(TextBox const& tb, Canvas c) -> void;

auto resize(TextBox& tb, Area new_size) -> void;

auto key_press(TextBox& tb, Key k, Widget::Properties const& p) -> void;

}  // namespace ox::widgets
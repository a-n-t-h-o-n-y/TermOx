#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include <termox/core.hpp>
#include <termox/widgets/widget.hpp>

namespace ox::widgets {

struct TextBox {
    std::string text = "";
    bool editable = true;
    bool word_wrap = true;  // TODO

    std::size_t cursor_index = 0;
    std::size_t width = 0;

    // TODO Scrolling
    // std::size_t top_display_index = 0;  // index into text
};

auto paint(TextBox const& tb, Canvas c) -> void;

auto resize(TextBox& tb, Area new_size) -> void;

auto key_press(TextBox& tb, Key k) -> void;

auto cursor(TextBox const& tb) -> ox::Terminal::Cursor;

}  // namespace ox::widgets
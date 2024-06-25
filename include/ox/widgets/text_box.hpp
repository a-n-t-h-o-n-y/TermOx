#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include <ox/core.hpp>
#include <ox/widgets/widget.hpp>

namespace ox {

struct TextBox {
    std::string text;
    bool editable;
    bool word_wrap;  // TODO

    std::size_t cursor_index = 0;
    std::size_t width = 0;

    // TODO Scrolling
    // std::size_t top_display_index = 0;  // index into text
};

struct TextBoxInit {
    std::string text = "";
    bool editable = true;
    bool word_wrap = true;
};

[[nodiscard]] auto text_box(TextBoxInit x) -> TextBox;

auto paint(TextBox const& tb, Canvas c) -> void;

auto resize(TextBox& tb, Area new_size) -> void;

auto key_press(TextBox& tb, Key k) -> void;

auto cursor(TextBox const& tb) -> Terminal::Cursor;

}  // namespace ox
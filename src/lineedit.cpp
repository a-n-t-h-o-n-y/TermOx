#include <ox/lineedit.hpp>

#include <algorithm>
#include <cassert>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

#include <ox/core/core.hpp>
#include <ox/focus.hpp>
#include <ox/put.hpp>

namespace ox {

LineEdit::Options const LineEdit::init = {};

LineEdit::LineEdit(Options x)
    : Widget{x.focus_policy, x.size_policy},
      text{std::move(x.text)},
      text_style{std::move(x.text_style)},
      align{std::move(x.align)},
      background{std::move(x.background)},
      ghost_text{std::move(x.ghost_text)},
      ghost_text_style{std::move(x.ghost_text_style)},
      validator{std::move(x.validator)}
{}

void LineEdit::key_press(KeyEvent k)
{
    auto unicode = esc::utf8_to_utf32(text);
    cursor_index_ = std::min(cursor_index_, unicode.size());
    if (k.matches(KeyCode::Backspace, {}, k.action)) {
        if (cursor_index_ > 0) { unicode.erase(--cursor_index_, 1); }
    }
    else if (k.matches(KeyCode::Delete, {}, k.action)) {
        if (cursor_index_ < unicode.size()) { unicode.erase(cursor_index_, 1); }
    }
    else if (k.matches(KeyCode::Enter, {}, k.action)) {
        this->on_enter(text);
        return;
    }
    else if (k.matches(KeyCode::ArrowLeft, {}, k.action)) {
        if (cursor_index_ > 0) { --cursor_index_; }
    }
    else if (k.matches(KeyCode::ArrowRight, {}, k.action)) {
        cursor_index_ = std::min(cursor_index_ + 1, unicode.size());
    }
    else if (k.matches(KeyCode::Home, {}, k.action)) {
        cursor_index_ = 0;
    }
    else if (k.matches(KeyCode::End, {}, k.action)) {
        cursor_index_ = unicode.size();
    }
    else {
        auto input = input_text(k);
        if (validator) {
            std::erase_if(input, [&](char32_t c) { return !(*validator)(c); });
        }
        unicode.insert(cursor_index_, input);
        cursor_index_ += input.size();
    }
    text = esc::utf32_to_utf8(unicode);
    this->update_scroll_offset();
}

void LineEdit::paste(esc::PasteChunk const& chunk)
{
    if (chunk.first) { paste_buffer_.clear(); }
    paste_buffer_ += chunk.bytes;
    if (chunk.last) {
        if (!chunk.aborted) {
            this->key_press(KeyEvent{.text = esc::utf8_to_utf32(paste_buffer_)});
        }
        paste_buffer_.clear();
    }
}

void LineEdit::mouse_press(MousePress m)
{
    cursor_index_ = std::min(anchor_index_ + (std::size_t)std::max(m.at.x, 0),
                             esc::utf8_to_utf32(text).size());
}

void LineEdit::paint(Canvas c)
{
    auto const unicode = esc::utf8_to_utf32(text);
    auto const ghost = esc::utf8_to_utf32(ghost_text);
    anchor_index_ = std::min(anchor_index_, unicode.size());
    cursor_index_ = std::min(cursor_index_, unicode.size());

    auto const is_ghost = text.empty() && !this->focused;

    auto offset = std::size_t{0};
    for (auto cluster : esc::graphemes(unicode)) {
        if (anchor_index_ < offset + cluster.size()) {
            anchor_index_ = offset;
            break;
        }
        offset += cluster.size();
    }
    auto const trim = [this](std::u32string_view text) {
        return text.substr(std::min(anchor_index_, text.size()));
    };

    fill(c, bg(background));

    auto const for_screen = is_ghost ? esc::glyphs(trim(ghost)) | ghost_text_style
                                     : esc::glyphs(trim(unicode)) | text_style;

    put(c, {.x = 0, .y = 0}, for_screen);

    // Map the scalar cursor to the leading edge of its grapheme in terminal columns.
    auto columns = std::size_t{0};
    offset = anchor_index_;
    for (auto cluster : esc::graphemes(trim(unicode))) {
        if (offset + cluster.size() > cursor_index_) { break; }
        auto const width = esc::grapheme_width(cluster, c.width_policy());
        if (!width) { throw std::invalid_argument{"LineEdit requires printable text"}; }
        columns += *width;
        offset += cluster.size();
    }
    Widget::cursor =
        cursor_index_ < anchor_index_ || columns >= (std::size_t)c.size().width
            ? Cursor{}
            : Cursor{Point{(int)columns, 0}};
}

void LineEdit::update_scroll_offset()
{
    auto const width = (std::size_t)std::max(this->size.width, 1);
    if (cursor_index_ >= anchor_index_ + width) {
        anchor_index_ = cursor_index_ - width + 1;
    }
    else if (cursor_index_ < anchor_index_) {
        anchor_index_ = cursor_index_;
    }
}

}  // namespace ox
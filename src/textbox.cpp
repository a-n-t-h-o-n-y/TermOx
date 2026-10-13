#include <ox/textbox.hpp>

#include <algorithm>
#include <cstdint>
#include <limits>

#include <ox/put.hpp>
#include <ox/scrollbar.hpp>

namespace ox {
TextBox::Options const TextBox::init = {};

TextBox::TextBox(Options x)
    : Widget{x.focus_policy, x.size_policy},
      editor{x.text},
      wrap{x.wrap},
      align{x.align},
      style{x.style},
      selection_style{x.selection_style}
{}

void TextBox::set_text(std::string_view text)
{
    editor.set_text(text);
    scroll_offset_ = 0;
    request_repaint();
}
void TextBox::update_layout() const
{ layout_.update(editor.text(), size.width, wrap, layout_.policy()); }

auto TextBox::get_scroll_length() const -> int
{
    update_layout();
    return (int)layout_.lines().size();
}

void TextBox::notify_scroll()
{
    auto const count = (int)layout_.lines().size();
    if (notified_offset_ == scroll_offset_ && notified_length_ == count) { return; }
    notified_offset_ = scroll_offset_;
    notified_length_ = count;
    on_scroll(scroll_offset_, count);
}

void TextBox::keep_caret_visible()
{
    scroll_offset_ = std::clamp(scroll_offset_, 0, (int)layout_.lines().size() - 1);
    if (size.height <= 0) { return; }
    auto const caret = layout_.position(editor.caret()).y;
    if (caret < scroll_offset_) { scroll_offset_ = caret; }
    else if (caret - scroll_offset_ >= size.height) {
        scroll_offset_ = caret - size.height + 1;
    }
}

auto TextBox::set_scroll_offset(int position) -> bool
{
    update_layout();
    auto const old = scroll_offset_;
    auto const current = layout_.position(editor.caret());
    scroll_offset_ = std::clamp(position, 0, (int)layout_.lines().size() - 1);
    auto changed = old != scroll_offset_;
    if (size.height > 0) {
        auto const last = (int)std::min(std::int64_t{scroll_offset_} + size.height - 1,
                                        (std::int64_t)layout_.lines().size() - 1);
        auto const row = std::clamp(current.y, scroll_offset_, last);
        if (row != current.y) {
            changed |= editor.set_caret(layout_.offset({current.x, row}));
        }
    }
    if (changed) { request_repaint(); }
    notify_scroll();
    return changed;
}

auto TextBox::key_press(KeyEvent key) -> EventResponse
{
    update_layout();
    auto const response = editor.key_press(key, layout_);
    if (response.repaint) {
        update_layout();
        keep_caret_visible();
        notify_scroll();
    }
    return response;
}

auto TextBox::paste(esc::PasteChunk const& chunk) -> EventResponse
{
    auto const changed = editor.paste(chunk);
    if (changed) {
        update_layout();
        keep_caret_visible();
        notify_scroll();
    }
    return {.handled = true, .repaint = changed};
}

auto TextBox::mouse_press(MousePress mouse) -> EventResponse
{
    if (mouse.button != MouseButton::Left) { return {}; }
    update_layout();
    auto const row =
        (int)std::clamp(std::int64_t{scroll_offset_} + mouse.at.y, std::int64_t{0},
                        (std::int64_t)layout_.lines().size() - 1);
    auto const offset =
        find_align_offset(align, size.width, layout_.lines()[(std::size_t)row].columns);
    auto const changed = editor.set_caret(
        layout_.offset(
            {(int)std::clamp(std::int64_t{mouse.at.x} - offset, std::int64_t{0},
                             (std::int64_t)std::numeric_limits<int>::max()),
             row}),
        mouse.modifiers.shift);
    return {.handled = true, .repaint = changed};
}

auto TextBox::mouse_wheel(MouseWheel mouse) -> EventResponse
{
    if (mouse.dy == 0) { return {}; }
    auto const position =
        (int)std::clamp(std::int64_t{scroll_offset_} + mouse.dy, std::int64_t{0},
                        (std::int64_t)get_scroll_length() - 1);
    auto const changed = set_scroll_offset(position);
    return {.handled = true, .repaint = changed};
}

void TextBox::paint(Canvas canvas)
{
    layout_.update(editor.text(), canvas.size().width, wrap, canvas.width_policy());
    keep_caret_visible();
    fill(canvas, style);
    for (auto y = 0;
         y < canvas.size().height && y < (int)layout_.lines().size() - scroll_offset_;
         ++y) {
        auto const row = (std::size_t)(scroll_offset_ + y);
        auto const x =
            find_align_offset(align, canvas.size().width, layout_.lines()[row].columns);
        put_text_line(canvas, {x, y}, layout_, row, style, style | selection_style,
                      editor.selection());
    }
    auto point = layout_.position(editor.caret());
    point.x += find_align_offset(align, canvas.size().width,
                                 layout_.lines()[(std::size_t)point.y].columns);
    point.y -= scroll_offset_;
    cursor = canvas.contains(point) ? Cursor{point} : Cursor{};
    notify_scroll();
}

void link(TextBox& tb, ScrollBar& sb)
{
    Connection{
        .signal = tb.on_scroll,
        .slot =
            [](int pos, int len, ScrollBar& sb) {
                sb.position = pos;
                sb.scrollable_length = len;
            },
    }(sb);

    Connection{
        .signal = sb.on_scroll,
        .slot = [](int pos, TextBox& tb) { tb.set_scroll_offset(pos); },
    }(tb);
}

}  // namespace ox

#include <ox/textbox.hpp>

#include <algorithm>
#include <cstdint>

#include <ox/put.hpp>
#include <ox/scroll.hpp>
#include <ox/scrollbar.hpp>
#include <ox/text_view.hpp>

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
    scroll_ = {};
    update_layout();
    request_repaint();
    notify_scroll();
}
void TextBox::on_arrange(Rect)
{
    update_layout();
    scroll_ = text_scroll(layout_, editor.caret(), size(), align, scroll_);
    notify_scroll();
}
void TextBox::update_layout() const
{ layout_.update(editor.text(), size().width, wrap, layout_.policy()); }

auto TextBox::get_scroll_length() const -> int
{
    update_layout();
    return (int)layout_.lines().size();
}

void TextBox::notify_scroll()
{
    auto const count = (int)layout_.lines().size();
    if (notified_offset_ == scroll_.y && notified_length_ == count &&
        notified_viewport_ == size().height) {
        return;
    }
    notified_offset_ = scroll_.y;
    notified_length_ = count;
    notified_viewport_ = size().height;
    on_scroll(scroll_.y, count, size().height);
}

auto TextBox::set_scroll_offset(int position) -> bool
{
    update_layout();
    auto const old = scroll_;
    auto const current = layout_.position(editor.caret());
    scroll_.y = clamp_scroll(position, (int)layout_.lines().size(), size().height);
    auto changed = old.y != scroll_.y;
    if (size().height > 0) {
        auto const last = (int)std::min(std::int64_t{scroll_.y} + size().height - 1,
                                        (std::int64_t)layout_.lines().size() - 1);
        auto const row = std::clamp(current.y, scroll_.y, last);
        if (row != current.y) {
            changed |= editor.set_caret(layout_.offset({current.x, row}));
        }
    }
    scroll_ = text_scroll(layout_, editor.caret(), size(), align, scroll_);
    changed |= old != scroll_;
    if (changed) { request_repaint(); }
    notify_scroll();
    return changed;
}

auto TextBox::handle(KeyEvent const& key) -> EventResponse
{
    if (key.action == KeyAction::Release) { return {}; }
    auto const response = edit_text(editor, layout_, scroll_, size(), wrap, align, key);
    if (response.repaint) { notify_scroll(); }
    return response;
}

auto TextBox::handle(esc::PasteChunk const& chunk) -> EventResponse
{
    auto const response =
        edit_text(editor, layout_, scroll_, size(), wrap, align, chunk);
    if (response.repaint) { notify_scroll(); }
    return response;
}

auto TextBox::handle(MousePress const& mouse) -> EventResponse
{
    if (mouse.button != MouseButton::Left) { return {}; }
    update_layout();
    auto const changed = editor.set_caret(
        text_offset(layout_, mouse.at, size(), align, scroll_), mouse.modifiers.shift);
    return {.handled = true, .repaint = changed};
}

auto TextBox::handle(MouseWheel const& mouse) -> EventResponse
{
    if (mouse.dy == 0) { return {}; }
    auto const position =
        scroll_by(scroll_.y, mouse.dy, get_scroll_length(), size().height);
    auto const changed = set_scroll_offset(position);
    return {.handled = true, .repaint = changed};
}

auto TextBox::paint(Canvas canvas) -> Cursor
{
    layout_.update(editor.text(), canvas.size().width, wrap, canvas.width_policy());
    scroll_ = text_scroll(layout_, editor.caret(), canvas.size(), align, scroll_);
    fill(canvas, style);
    paint_text(canvas, layout_, align, scroll_, style, style | selection_style,
               editor.selection());
    auto const point =
        text_position(layout_, editor.caret(), canvas.size(), align, scroll_);
    notify_scroll();
    return canvas.contains(point) ? Cursor{point} : Cursor{};
}

void link(TextBox& tb, ScrollBar& sb)
{
    sb.vertical = true;
    auto synchronize = [](TextBox& box, ScrollBar& bar) {
        bar.set_range(box.get_scroll_length(), box.size().height,
                      box.scroll_offset().y);
    };
    synchronize(tb, sb);
    Connection{.signal = tb.on_scroll,
               .slot = [synchronize](int, int, int, TextBox& box, ScrollBar& bar) {
                   synchronize(box, bar);
               }}(tb, sb);
    Connection{.signal = sb.on_scroll, .slot = [](int, TextBox& box, ScrollBar& bar) {
                   box.set_scroll_offset(bar.position);
               }}(tb, sb);
}
}  // namespace ox

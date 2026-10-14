#include <ox/lineedit.hpp>

#include <algorithm>
#include <cstdint>
#include <limits>

#include <ox/put.hpp>

namespace ox {
LineEdit::Options const LineEdit::init = {};

LineEdit::LineEdit(Options x)
    : Widget{x.focus_policy, x.size_policy},
      editor{x.text, false},
      text_style{x.text_style},
      align{x.align},
      selection_style{x.selection_style},
      background{x.background},
      ghost_text{std::move(x.ghost_text)},
      ghost_text_style{x.ghost_text_style},
      validator{std::move(x.validator)}
{}

void LineEdit::set_text(std::string_view text)
{
    editor.set_text(text);
    scroll_column_ = 0;
    request_repaint();
}
void LineEdit::update_layout()
{ layout_.update(editor.text(), size().width, TextWrap::None, layout_.policy()); }

auto LineEdit::display_offset(int width) -> int
{
    auto const columns = layout_.lines().front().columns;
    auto const caret = layout_.position(editor.caret()).x;
    if (width <= 0) {
        scroll_column_ = 0;
        return 0;
    }
    if (columns < width) {
        scroll_column_ = 0;
        return find_align_offset(align, width, columns);
    }
    scroll_column_ = std::clamp(scroll_column_, 0, columns);
    if (caret < scroll_column_) { scroll_column_ = caret; }
    if (caret - scroll_column_ >= width) {
        auto const desired = caret - width + 1;
        auto offset = layout_.offset({desired, 0});
        if (layout_.position(offset).x < desired) {
            offset = *std::ranges::upper_bound(editor.boundaries(), offset);
        }
        scroll_column_ = layout_.position(offset).x;
    }
    return -scroll_column_;
}

auto LineEdit::handle(KeyEvent const& key) -> EventResponse
{
    if (key.action == KeyAction::Release) { return {}; }
    if (key.matches(KeyCode::Enter, {}, key.action)) {
        auto const value = editor.text();  // Slots may replace the editor's text.
        on_enter(value);
        return {.handled = true, .repaint = true};
    }
    update_layout();
    auto response = edit_key(editor, key, layout_,
                             validator.value_or(std::function<bool(char32_t)>{}));
    if (response.repaint) {
        update_layout();
        display_offset(size().width);
    }
    return response;
}

auto LineEdit::handle(esc::PasteChunk const& chunk) -> EventResponse
{
    auto const changed =
        editor.paste(chunk, validator.value_or(std::function<bool(char32_t)>{}));
    if (changed) {
        update_layout();
        display_offset(size().width);
    }
    return {.handled = true, .repaint = changed};
}

auto LineEdit::handle(MousePress const& mouse) -> EventResponse
{
    if (mouse.button != MouseButton::Left) { return {}; }
    update_layout();
    auto const offset = display_offset(size().width);
    auto const changed = editor.set_caret(
        layout_.offset(
            {(int)std::clamp(std::int64_t{mouse.at.x} - offset, std::int64_t{0},
                             (std::int64_t)std::numeric_limits<int>::max()),
             0}),
        mouse.modifiers.shift);
    return {.handled = true, .repaint = changed};
}

auto LineEdit::paint(Canvas canvas) -> Cursor
{
    layout_.update(editor.text(), canvas.size().width, TextWrap::None,
                   canvas.width_policy());
    auto const offset = display_offset(canvas.size().width);
    fill(canvas, bg(background));
    if (editor.text().empty() && !is_focused()) {
        ghost_layout_.update(ghost_text, canvas.size().width, TextWrap::None,
                             canvas.width_policy());
        auto const x = find_align_offset(align, canvas.size().width,
                                         ghost_layout_.lines().front().columns);
        put_text_line(canvas, {x, 0}, ghost_layout_, 0, ghost_text_style,
                      ghost_text_style);
    }
    else {
        put_text_line(canvas, {offset, 0}, layout_, 0, text_style,
                      text_style | selection_style, editor.selection());
    }
    auto const point = Point{layout_.position(editor.caret()).x + offset, 0};
    return canvas.contains(point) ? Cursor{point} : Cursor{};
}
}  // namespace ox

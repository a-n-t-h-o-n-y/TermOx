#include <ox/lineedit.hpp>

#include <ox/put.hpp>
#include <ox/text_view.hpp>

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
    scroll_ = {};
    request_repaint();
}
void LineEdit::update_layout()
{ layout_.update(editor.text(), size().width, TextWrap::None, layout_.policy()); }

auto LineEdit::handle(KeyEvent const& key) -> EventResponse
{
    if (key.action == KeyAction::Release) { return {}; }
    if (key.matches(KeyCode::Enter, {}, key.action)) {
        auto const value = editor.text();  // Slots may replace the editor's text.
        on_enter(value);
        return {.handled = true, .repaint = true};
    }
    return edit_text(editor, layout_, scroll_, size(), TextWrap::None, align, key,
                     validator);
}

auto LineEdit::handle(esc::PasteChunk const& chunk) -> EventResponse
{
    return edit_text(editor, layout_, scroll_, size(), TextWrap::None, align, chunk,
                     validator);
}

auto LineEdit::handle(MousePress const& mouse) -> EventResponse
{
    if (mouse.button != MouseButton::Left) { return {}; }
    update_layout();
    scroll_ = text_scroll(layout_, editor.caret(), size(), align, scroll_);
    auto const changed =
        editor.set_caret(text_offset(layout_, {mouse.at.x, 0}, size(), align, scroll_),
                         mouse.modifiers.shift);
    return {.handled = true, .repaint = changed};
}

auto LineEdit::paint(Canvas canvas) -> Cursor
{
    layout_.update(editor.text(), canvas.size().width, TextWrap::None,
                   canvas.width_policy());
    scroll_ = text_scroll(layout_, editor.caret(), canvas.size(), align, scroll_);
    fill(canvas, bg(background));
    auto const row = canvas.subcanvas(
        {0, 0}, {canvas.size().width, canvas.size().height > 0 ? 1 : 0});
    if (editor.text().empty() && !is_focused()) {
        ghost_layout_.update(ghost_text, canvas.size().width, TextWrap::None,
                             canvas.width_policy());
        paint_text(row, ghost_layout_, align, {}, ghost_text_style, ghost_text_style);
    }
    else {
        paint_text(row, layout_, align, scroll_, text_style,
                   text_style | selection_style, editor.selection());
    }
    auto const point =
        text_position(layout_, editor.caret(), canvas.size(), align, scroll_);
    return canvas.contains(point) ? Cursor{point} : Cursor{};
}
}  // namespace ox

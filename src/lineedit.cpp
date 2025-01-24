#include <ox/lineedit.hpp>

#include <algorithm>
#include <cassert>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

#include <ox/core/core.hpp>
#include <ox/focus.hpp>

namespace ox {

LineEdit::Init const LineEdit::init = {};

LineEdit::LineEdit(Init state)
    : Widget{FocusPolicy::Strong, SizePolicy::flex()},
      text{std::move(state.text)},
      text_brush{state.text_brush},
      align{state.align},
      background{state.background},
      ghost_text{std::move(state.ghost_text)},
      ghost_text_brush{state.ghost_text_brush},
      validator{std::move(state.validator)}
{}

void LineEdit::key_press(Key k)
{
    switch (k) {
        case Key::Backspace:
            if (!text.empty() && cursor_index_ > 0) {
                this->increment_cursor(-1);
                text.erase(std::next(std::cbegin(text), (std::ptrdiff_t)cursor_index_));
            }
            break;
        case Key::Delete:
            if (cursor_index_ < text.size()) {
                text.erase(std::next(std::cbegin(text), (std::ptrdiff_t)cursor_index_));
            }
            break;
        case Key::Enter: this->on_enter(text); break;
        case Key::ArrowLeft: this->increment_cursor(-1); break;
        case Key::ArrowRight: this->increment_cursor(1); break;
        case Key::Home:
            cursor_index_ = 0;
            anchor_index_ = 0;
            break;
        case Key::End:
            cursor_index_ = text.size();
            anchor_index_ =
                (std::size_t)std::max((int)text.size() - this->size.width + 1, 0);
            break;
        default:
            if (auto const c = esc::key_to_char(k); c != '\0') {
                if (!validator || (*validator)(c)) {
                    text.insert(
                        std::next(std::begin(text), (std::ptrdiff_t)cursor_index_), c);
                    this->increment_cursor(1);
                }
            }
            break;
    }
}

void LineEdit::mouse_press(Mouse m)
{
    cursor_index_ = std::min(anchor_index_ + (std::size_t)m.at.x, text.size());
}

void LineEdit::paint(Canvas c)
{
    anchor_index_ = std::min(anchor_index_, text.size());
    cursor_index_ = std::min(cursor_index_, text.size());

    auto const is_ghost = [this] {
        Widget const* const focused = [] {
            auto const life = Focus::get();
            return life.valid() ? &life.get() : nullptr;
        }();
        return text.empty() && focused != this;
    }();

    auto const trim = [this](std::string_view sv) -> std::string_view {
        return sv.substr(anchor_index_,
                         (std::size_t)std::min(this->size.width,
                                               (int)sv.size() - (int)anchor_index_));
    };

    Painter{c}[{0, 0}] << (trim(is_ghost ? ghost_text : text) |
                           (is_ghost ? ghost_text_brush : text_brush));

    // Cursor
    if ((int)cursor_index_ >= (int)anchor_index_ + this->size.width ||
        cursor_index_ < anchor_index_) {
        Widget::cursor = std::nullopt;
    }
    else {
        Widget::cursor = Point{.x = (int)cursor_index_ - (int)anchor_index_, .y = 0};
    }
}

void LineEdit::increment_cursor(int amount)
{
    // Increment
    cursor_index_ =
        (std::size_t)std::clamp(0, (int)cursor_index_ + amount, (int)text.size());

    // Scroll
    if (cursor_index_ >= anchor_index_ + (std::size_t)this->size.width) {
        anchor_index_ = cursor_index_ - (std::size_t)this->size.width + 1;
    }
    else if (cursor_index_ < anchor_index_) {
        anchor_index_ = cursor_index_;
    }
}

}  // namespace ox
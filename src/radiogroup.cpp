#include <ox/radiogroup.hpp>

#include <ox/put.hpp>

namespace ox {

RadioGroup::Options const RadioGroup::init = {};

RadioGroup::RadioGroup(Options x)
    : Widget{x.focus_policy, x.size_policy},
      items{std::move(x.items)},
      selected_index{x.selected_index},
      style{x.style},
      in_focus_style{x.in_focus_style},
      on{x.on},
      off{x.off}
{}

void RadioGroup::paint(Canvas c)
{
    for (auto y = 0; y < (int)items.size(); ++y) {
        if (y >= c.size.height) { break; }
        auto& btn = (y == selected_index) ? on : off;
        put(c, {0, y}, btn);
        auto& b = in_focus_ && (y == focus_selection_) ? in_focus_style : style;
        put(c, {(int)btn.size(), y}, esc::glyphs(' ' + items[(std::size_t)y]) | b);
    }
}

void RadioGroup::key_press(KeyEvent k)
{
    if (items.empty()) { return; }
    if (k.matches(KeyCode::Enter, {}, k.action)) {
        if (focus_selection_) {
            selected_index =
                std::clamp(*focus_selection_, (std::size_t)0, items.size());
            this->on_select(items[*selected_index]);
        }
    }
    else if (k.matches(KeyCode::ArrowUp, {}, k.action)) {
        focus_selection_ =
            std::clamp(focus_selection_ ? ((int)*focus_selection_ - 1) : 0, 0,
                       (int)items.size() - 1);
    }
    else if (k.matches(KeyCode::ArrowDown, {}, k.action)) {
        focus_selection_ =
            std::clamp(focus_selection_ ? ((int)*focus_selection_ + 1) : 0, 0,
                       (int)items.size() - 1);
    }
}

void RadioGroup::mouse_press(MousePress m)
{
    if (m.at.y < (int)items.size() && m.at.y != selected_index) {
        selected_index = m.at.y;
        focus_selection_ = selected_index;
        this->on_select(items[*selected_index]);
    }
}

void RadioGroup::focus_in() { in_focus_ = true; }

void RadioGroup::focus_out() { in_focus_ = false; }

}  // namespace ox
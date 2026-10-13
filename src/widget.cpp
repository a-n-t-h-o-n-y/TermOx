#include <ox/widget.hpp>

#include <memory>

#include <ox/focus.hpp>

namespace ox {

Widget::Widget(FocusPolicy fp, SizePolicy sp) : focus_policy{fp}, size_policy{sp} {}

Widget::Widget(Widget&& other)
    : focus_policy{other.focus_policy},
      size_policy{other.size_policy},
      cursor{other.cursor},
      active{other.active},
      focused{other.focused},
      at{other.at},
      size{other.size},
      lifetime{std::move(other.lifetime)},
      application_{std::exchange(other.application_, nullptr)},
      timers_{std::move(other.timers_)}
{ *lifetime = this; }

auto Widget::operator=(Widget&& other) -> Widget&
{
    focus_policy = other.focus_policy;
    size_policy = other.size_policy;
    cursor = other.cursor;
    active = other.active;
    focused = other.focused;
    at = other.at;
    size = other.size;

    application_ = std::exchange(other.application_, nullptr);
    timers_ = std::move(other.timers_);
    lifetime = std::move(other.lifetime);
    *lifetime = this;

    return *this;
}

}  // namespace ox
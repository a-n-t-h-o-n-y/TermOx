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
      enabled{other.enabled},
      focused{std::exchange(other.focused, false)},
      at{other.at},
      size{other.size},
      lifetime{std::move(other.lifetime)},
      application_{std::exchange(other.application_, nullptr)},
      timers_{std::move(other.timers_)}
{
    if (!lifetime) { lifetime = std::make_shared<Widget*>(this); }
    *lifetime = this;
}

auto Widget::operator=(Widget&& other) -> Widget&
{
    if (this == &other) { return *this; }
    if (lifetime) { *lifetime = nullptr; }
    for (auto const& entry : timers_) {
        if (auto timer = entry.lock()) {
            timer->deadline.reset();
            timer->owner = nullptr;
        }
    }
    focus_policy = other.focus_policy;
    size_policy = other.size_policy;
    cursor = other.cursor;
    active = other.active;
    enabled = other.enabled;
    focused = std::exchange(other.focused, false);
    at = other.at;
    size = other.size;

    application_ = std::exchange(other.application_, nullptr);
    timers_ = std::move(other.timers_);
    lifetime = std::move(other.lifetime);
    if (!lifetime) { lifetime = std::make_shared<Widget*>(this); }
    *lifetime = this;

    return *this;
}

Widget::~Widget()
{
    for (auto const& entry : timers_) {
        if (auto timer = entry.lock()) {
            timer->deadline.reset();
            timer->owner = nullptr;
        }
    }
    if (lifetime) { *lifetime = nullptr; }
}

}  // namespace ox
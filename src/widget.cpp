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
      at{other.at},
      size{other.size},
      lifetime{std::move(other.lifetime)}
{
    *lifetime = this;
}

auto Widget::operator=(Widget&& other) -> Widget&
{
    focus_policy = other.focus_policy;
    size_policy = other.size_policy;
    cursor = other.cursor;
    active = other.active;
    at = other.at;
    size = other.size;

    lifetime = std::move(other.lifetime);
    *lifetime = this;

    return *this;
}

}  // namespace ox
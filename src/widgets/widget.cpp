#include <ox/widgets/widget.hpp>

#include <memory>

#include <ox/widgets/focus.hpp>

namespace ox {

Widget::Widget(Widget&& other)
    : focus_policy{other.focus_policy},
      cursor{other.cursor},
      enabled{other.enabled},
      at{other.at},
      size{other.size},
      lifetime{other.lifetime}
{
    *lifetime = this;
}

auto Widget::operator=(Widget&& other) -> Widget&
{
    focus_policy = other.focus_policy;
    cursor = other.cursor;
    enabled = other.enabled;
    at = other.at;
    size = other.size;

    lifetime = other.lifetime;
    *lifetime = this;

    return *this;
}

}  // namespace ox
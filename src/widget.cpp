#include <ox/widget.hpp>

#include <memory>

#include <ox/focus.hpp>

namespace ox {

auto SizePolicy::fixed(int size) -> SizePolicy
{
    return {.minimum = size, .maximum = size, .flexibility = 0.f};
}

auto SizePolicy::flex(float flex) -> SizePolicy
{
    return {
        .minimum = 0, .maximum = std::numeric_limits<int>::max(), .flexibility = flex};
}

auto SizePolicy::bounded(int min, int max) -> SizePolicy
{
    return {.minimum = min, .maximum = max, .flexibility = 1.f};
}

auto SizePolicy::min(int min) -> SizePolicy
{
    return {
        .minimum = min, .maximum = std::numeric_limits<int>::max(), .flexibility = 1.f};
}

auto SizePolicy::max(int max) -> SizePolicy
{
    return {.minimum = 0, .maximum = max, .flexibility = 1.f};
}

// -------------------------------------------------------------------------------------

Widget::Widget(Widget&& other)
    : focus_policy{other.focus_policy},
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
    cursor = other.cursor;
    active = other.active;
    at = other.at;
    size = other.size;

    lifetime = std::move(other.lifetime);
    *lifetime = this;

    return *this;
}

}  // namespace ox
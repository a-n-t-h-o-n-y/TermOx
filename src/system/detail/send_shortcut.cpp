#include <termox/system/detail/send_shortcut.hpp>

#include <termox/system/event.hpp>
#include <termox/system/shortcuts.hpp>

namespace ox::detail {

template <>
auto send_shortcut<ox::Key_press_event>(ox::Key_press_event const& e) -> bool
{
    return ox::Shortcuts::send_key(e.key);
}

}  // namespace ox::detail

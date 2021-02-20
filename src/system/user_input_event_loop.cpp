#include <termox/system/detail/user_input_event_loop.hpp>

#include <termox/system/event.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/terminal.hpp>

namespace ox::detail {

auto User_input_event_loop::run() -> int
{
    return loop_.run(
        [] { ox::System::post_event(ox::Terminal::read_input()); });
}

}  // namespace ox::detail

#include <termox/system/detail/user_input_event_loop.hpp>

#include <termox/system/event_queue.hpp>
#include <termox/terminal/terminal.hpp>
#include <termox/widget/widget.hpp>

namespace ox::detail {

auto User_input_event_loop::run() -> int
{
    return loop_.run(
        [](Event_queue& queue) { queue.append(ox::Terminal::read_input()); });
}

}  // namespace ox::detail

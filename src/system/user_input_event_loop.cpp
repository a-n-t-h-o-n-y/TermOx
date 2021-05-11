#include <termox/system/detail/user_input_event_loop.hpp>

#include <termox/system/event.hpp>
#include <termox/system/event_queue.hpp>
#include <termox/terminal/terminal.hpp>
#include <termox/widget/widget.hpp>

namespace ox::detail {

auto User_input_event_loop::run() -> int
{
    return loop_.run(
        [](Event_queue& q) { q.append(ox::Terminal::read_input()); });
}

void User_input_event_loop::exit(int exit_code) { loop_.exit(exit_code); }

auto User_input_event_loop::event_queue() -> Event_queue&
{
    return loop_.event_queue();
}

}  // namespace ox::detail

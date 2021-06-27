#include <termox/system/event_loop.hpp>

namespace ox {

void Event_loop::exit(int return_code)
{
    return_code_ = return_code;
    exit_        = true;
}

auto Event_loop::wait() -> int
{
    if (!fut_.valid())
        return -1;
    return fut_.get();
}

auto Event_loop::is_running() const -> bool { return running_; }

auto Event_loop::exit_flag() const -> bool { return exit_; }

auto Event_loop::event_queue() -> Event_queue& { return queue_; }

auto Event_loop::event_queue() const -> Event_queue const& { return queue_; }

}  // namespace ox

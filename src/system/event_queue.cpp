#include <termox/system/event_queue.hpp>

#include <mutex>
#include <utility>

#include <termox/system/system.hpp>
#include <termox/terminal/terminal.hpp>
#include <termox/widget/widget.hpp>

namespace ox::detail {

auto Paint_queue::send_all() -> bool
{
    events_.compress();
    /// Processing Paint_events should not post more Paint_events.
    bool sent = false;
    for (auto& p : events_)
        sent = System::send_event(std::move(p)) || sent;
    events_.clear();
    return sent;
}

auto Delete_queue::send_all() -> bool
{
    /// Processing Delete_events should not post more Delete_events.
    for (auto& d : deletes_)
        System::send_event(std::move(d));
    deletes_.clear();
    return true;
}

auto Basic_queue::send_all() -> bool
{
    // Allows for send(e) appending to the queue and invalidating iterators.
    bool sent = false;
    for (auto index = 0uL; index < basics_.size(); ++index)
        sent = System::send_event(std::move(basics_[index])) || sent;
    basics_.clear();
    return sent;
}
}  // namespace ox::detail

namespace ox {

void Event_queue::send_all()
{
    static auto mtx = std::mutex{};
    auto const lock = std::lock_guard{mtx};
    System::set_current_queue(*this);
    bool sent = basics_.send_all();
    sent      = paints_.send_all() || sent;
    sent      = deletes_.send_all() || sent;
    if (sent)
        Terminal::flush_screen();
}

}  // namespace ox

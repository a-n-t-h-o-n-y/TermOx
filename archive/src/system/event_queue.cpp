#include <termox/system/event_queue.hpp>

#include <cstddef>
#include <memory>
#include <mutex>
#include <utility>
#include <variant>

#include <termox/system/event.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/terminal.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

auto operator<(Paint_event const& x, Paint_event const& y) -> bool
{
    return std::addressof(x.receiver.get()) < std::addressof(y.receiver.get());
}

auto operator==(Paint_event const& a, Paint_event const& b) -> bool
{
    return std::addressof(a.receiver.get()) == std::addressof(b.receiver.get());
}

}  // namespace ox

namespace ox::detail {

void Paint_queue::append(Paint_event e) { events_.append(e); }

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

auto Paint_queue::size() const -> std::size_t { return events_.size(); }

void Delete_queue::append(Delete_event e) { deletes_.push_back(std::move(e)); }

void Delete_queue::send_all()
{
    /// Processing Delete_events should not post more Delete_events.
    for (auto& d : deletes_)
        System::send_event(std::move(d));
    deletes_.clear();
}

auto Delete_queue::size() const -> std::size_t { return deletes_.size(); }

void Basic_queue::append(Event e) { basics_.push_back(std::move(e)); }

auto Basic_queue::send_all() -> bool
{
    // Allows for send(e) appending to the queue and invalidating iterators.
    bool sent = false;
    for (auto index = 0uL; index < basics_.size(); ++index)
        sent = System::send_event(std::move(basics_[index])) || sent;
    basics_.clear();
    return sent;
}

auto Basic_queue::size() const -> std::size_t { return basics_.size(); }

}  // namespace ox::detail

namespace ox {

void Event_queue::append(Event e)
{
    std::visit(
        [this](auto&& e) {
            this->add_to_a_queue(std::forward<decltype(e)>(e));
        },
        std::move(e));
}

void Event_queue::send_all()
{
    // If widget tree has not been fully created yet, then do not process events
    // this prevents set_current_queue() from being called, while widget
    // constructors are still posting to the System::current_queue_, which
    // should be the user input event loop at construction. Prevents async loops
    // like the animation engine from assigning to current_queue during Widget
    // tree construction.
    if (System::head() == nullptr)
        return;
    static auto mtx = std::mutex{};
    auto const lock = std::lock_guard{mtx};
    System::set_current_queue(*this);
    bool sent = basics_.send_all();
    sent      = paints_.send_all() || sent;
    deletes_.send_all();
    if (sent)
        Terminal::flush_screen();
}

void Event_queue::add_to_a_queue(Paint_event e)
{
    paints_.append(std::move(e));
}

void Event_queue::add_to_a_queue(Delete_event e)
{
    deletes_.append(std::move(e));
}

}  // namespace ox

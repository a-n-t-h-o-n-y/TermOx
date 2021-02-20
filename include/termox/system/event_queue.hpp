#ifndef TERMOX_SYSTEM_EVENT_QUEUE_HPP
#define TERMOX_SYSTEM_EVENT_QUEUE_HPP
#include <memory>
#include <mutex>
#include <utility>
#include <variant>
#include <vector>

#include <termox/common/unique_queue.hpp>
#include <termox/system/event.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/terminal.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

inline auto operator<(Paint_event const& x, Paint_event const& y) -> bool
{
    return std::addressof(x.receiver.get()) < std::addressof(y.receiver.get());
}

inline auto operator==(Paint_event const& a, Paint_event const& b) -> bool
{
    return std::addressof(a.receiver.get()) == std::addressof(b.receiver.get());
}

}  // namespace ox

namespace ox::detail {

class Paint_queue {
   public:
    void append(Paint_event e) { events_.append(e); }

    /// Return true if any events are actually sent.
    auto send_all() -> bool
    {
        events_.compress();
        /// Processing Paint_events should not post more Paint_events.
        bool sent = false;
        for (auto& p : events_)
            sent = System::send_event(std::move(p)) || sent;
        events_.clear();
        return sent;
    }

    auto size() const -> std::size_t { return events_.size(); }

   private:
    Unique_queue<Paint_event> events_;
};

class Delete_queue {
   public:
    void append(Delete_event e) { deletes_.push_back(std::move(e)); }

    auto send_all() -> bool
    {
        /// Processing Delete_events should not post more Delete_events.
        for (auto& d : deletes_)
            System::send_event(std::move(d));
        deletes_.clear();
        return true;
    }

    auto size() const -> std::size_t { return deletes_.size(); }

   private:
    std::vector<Delete_event> deletes_;
};

class Basic_queue {
   public:
    void append(Event e) { basics_.push_back(std::move(e)); }

    auto send_all() -> bool
    {
        // Allows for send(e) appending to the queue and invalidating iterators.
        bool sent = false;
        for (auto index = 0uL; index < basics_.size(); ++index)
            sent = System::send_event(std::move(basics_[index])) || sent;
        basics_.clear();
        return sent;
    }

    auto size() const -> std::size_t { return basics_.size(); }

   private:
    std::vector<Event> basics_;
};

}  // namespace ox::detail

namespace ox {

class Event_queue {
   public:
    /// Adds the given event with priority for the underlying event type.
    static void append(Event e)
    {
        auto const lock = std::lock_guard{mtx_};
        std::visit(
            [](auto&& e) { add_to_a_queue(std::forward<decltype(e)>(e)); },
            std::move(e));
    }

    /// Send all events, then flush the screen if any events were actually sent.
    static void send_all()
    {
        auto const lock = std::lock_guard{mtx_};
        bool sent       = basics_.send_all();
        sent            = paints_.send_all() || sent;
        sent            = deletes_.send_all() || sent;
        if (sent)
            Terminal::flush_screen();
    }

   private:
    inline static detail::Basic_queue basics_;
    inline static detail::Paint_queue paints_;
    inline static detail::Delete_queue deletes_;

    inline static std::recursive_mutex mtx_;

   private:
    template <typename T>
    static void add_to_a_queue(T e)
    {
        basics_.append(std::move(e));
    }

    static void add_to_a_queue(Paint_event e) { paints_.append(std::move(e)); }

    static void add_to_a_queue(Delete_event e)
    {
        deletes_.append(std::move(e));
    }
};

}  // namespace ox
#endif  // TERMOX_SYSTEM_EVENT_QUEUE_HPP

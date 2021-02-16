#ifndef TERMOX_SYSTEM_DETAIL_EVENT_QUEUE_HPP
#define TERMOX_SYSTEM_DETAIL_EVENT_QUEUE_HPP
#include <memory>
#include <utility>
#include <variant>
#include <vector>

#include <termox/common/unique_queue.hpp>
#include <termox/system/event.hpp>
#include <termox/system/system.hpp>
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

    void send_all()
    {
        events_.compress();
        /// Processing Paint_events should not post more Paint_events.
        for (auto& p : events_)
            System::send_event(std::move(p));
        events_.clear();
    }

   private:
    Unique_queue<Paint_event> events_;
};

class Delete_queue {
   public:
    void append(Delete_event e) { deletes_.push_back(std::move(e)); }

    void send_all()
    {
        /// Processing Delete_events should not post more Delete_events.
        for (auto& d : deletes_)
            System::send_event(std::move(d));
        deletes_.clear();
    }

   private:
    std::vector<Delete_event> deletes_;
};

class Basic_queue {
   public:
    void append(Event e) { basics_.push_back(std::move(e)); }

    void send_all()
    {
        // Allows for send(e) appending to the queue and invalidating iterators.
        for (auto index = 0uL; index < basics_.size(); ++index)
            System::send_event(std::move(basics_[index]));
        basics_.clear();
    }

   private:
    std::vector<Event> basics_;
};

class Event_queue {
   public:
    /// Adds the given event with priority for the underlying event type.
    void append(Event e)
    {
        std::visit(
            [this](auto&& e) {
                this->add_to_a_queue(std::forward<decltype(e)>(e));
            },
            std::move(e));
    }

    void send_all()
    {
        basics_.send_all();
        paints_.send_all();
        deletes_.send_all();
    }

   private:
    Basic_queue basics_;
    Paint_queue paints_;
    Delete_queue deletes_;

   private:
    template <typename T>
    void add_to_a_queue(T e)
    {
        basics_.append(std::move(e));
    }

    void add_to_a_queue(Paint_event e) { paints_.append(std::move(e)); }

    void add_to_a_queue(Delete_event e) { deletes_.append(std::move(e)); }
};

}  // namespace ox::detail
#endif  // TERMOX_SYSTEM_DETAIL_EVENT_QUEUE_HPP

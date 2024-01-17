#ifndef TERMOX_SYSTEM_EVENT_QUEUE_HPP
#define TERMOX_SYSTEM_EVENT_QUEUE_HPP
#include <cstddef>
#include <utility>
#include <vector>

#include <termox/common/unique_queue.hpp>
#include <termox/system/event_fwd.hpp>

namespace ox {

[[nodiscard]] auto operator<(Paint_event const& x, Paint_event const& y)
    -> bool;

[[nodiscard]] auto operator==(Paint_event const& a, Paint_event const& b)
    -> bool;

}  // namespace ox

namespace ox::detail {

class Paint_queue {
   public:
    void append(Paint_event e);

    /// Return true if any events are actually sent.
    auto send_all() -> bool;

    [[nodiscard]] auto size() const -> std::size_t;

   private:
    Unique_queue<Paint_event> events_;
};

class Delete_queue {
   public:
    void append(Delete_event e);

    void send_all();

    [[nodiscard]] auto size() const -> std::size_t;

   private:
    std::vector<Delete_event> deletes_;
};

class Basic_queue {
   public:
    void append(Event e);

    auto send_all() -> bool;

    [[nodiscard]] auto size() const -> std::size_t;

   private:
    std::vector<Event> basics_;
};

}  // namespace ox::detail

namespace ox {

class Event_queue {
   public:
    /// Adds the given event with priority for the underlying event type.
    void append(Event e);

    /// Send all events, then flush the screen if any events were actually sent.
    void send_all();

   private:
    detail::Basic_queue basics_;
    detail::Paint_queue paints_;
    detail::Delete_queue deletes_;

   private:
    template <typename T>
    void add_to_a_queue(T e)
    {
        basics_.append(std::move(e));
    }

    void add_to_a_queue(Paint_event e);

    void add_to_a_queue(Delete_event e);
};

}  // namespace ox
#endif  // TERMOX_SYSTEM_EVENT_QUEUE_HPP

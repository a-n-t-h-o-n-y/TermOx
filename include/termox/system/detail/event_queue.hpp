#ifndef TERMOX_SYSTEM_DETAIL_EVENT_QUEUE_HPP
#define TERMOX_SYSTEM_DETAIL_EVENT_QUEUE_HPP
#include <memory>
#include <mutex>
#include <set>
#include <utility>
#include <variant>
#include <vector>

#include <termox/common/lockable.hpp>
#include <termox/system/event.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/widget.hpp>

namespace ox {
inline auto operator<(Paint_event const& x, Paint_event const& y) -> bool
{
    return std::addressof(x.receiver.get()) < std::addressof(y.receiver.get());
}
}  // namespace ox

namespace ox::detail {

class Paint_queue : public Lockable<std::mutex> {
   public:
    void append(Paint_event e)
    {
        auto const lock = this->Lockable::lock();
        paints_.insert(std::move(e));
    }

    void send_all()
    {
        auto const lock = this->Lockable::lock();
        for (auto& p : paints_)
            System::send_event(std::move(p));
        paints_.clear();
    }

   private:
    std::set<Paint_event> paints_;
};

class Delete_queue : public Lockable<std::mutex> {
   public:
    void append(Delete_event e)
    {
        auto const lock = this->Lockable::lock();
        deletes_.push_back(std::move(e));
    }

    void send_all()
    {
        auto const lock = this->Lockable::lock();
        for (auto& d : deletes_)
            System::send_event(std::move(d));
        deletes_.clear();
    }

   private:
    std::vector<Delete_event> deletes_;
};

// Mutex/Threading Notes
// The main thread is the only thread that can call Event_queue::send_all()
// Events are removed from the queue which a lock, then processed without a lock
// This does not require a recursive mutex because event is processed w/o lock.
// Paint and Delete Events should not be accessing functions that access the
// queue, so they can remain locked while processing.

class Basic_queue : public Lockable<std::mutex> {
   public:
    void append(Event e)
    {
        auto const lock = this->Lockable::lock();
        basics_.push_back(std::move(e));
    }

    void send_all()
    {
        // Allows for send(e) appending to the queue and invalidating iterators.
        for (auto index = this->get_begin_index(); index != -1uL;
             index      = this->increment_index(index)) {
            auto event = this->get_event(index);
            System::send_event(std::move(event));
        }
        auto const lock = this->Lockable::lock();
        basics_.clear();
    }

   private:
    std::vector<Event> basics_;

   private:
    auto get_event(std::size_t index) -> Event
    {
        auto const lock = this->Lockable::lock();
        return std::move(basics_[index]);
    }

    auto increment_index(std::size_t current) -> std::size_t
    {
        auto const lock = this->Lockable::lock();
        return current + 1uL < basics_.size() ? current + 1uL : -1uL;
    }

    auto get_begin_index() -> std::size_t
    {
        auto const lock = this->Lockable::lock();
        return basics_.empty() ? -1uL : 0uL;
    }
};

class Event_queue {
   public:
    /// Adds the given event with priority for the concrete event type.
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

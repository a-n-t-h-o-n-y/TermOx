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
   private:
    /// Holds a priority and a Paint_event.
    /** Priority is backwards, low values are higher priority. */
    struct Priority_paint_event {
        int priority;
        Paint_event event;
    };

    using Paint_event_container = std::vector<Priority_paint_event>;

   public:
    void append(Paint_event e)
    {
        auto const lock = this->Lockable::lock();
        // queue_.push_back({running_count_, e});
        // ++running_count_;
        paints_.insert(std::move(e));
    }

    void send_all()
    {
        auto const lock = this->Lockable::lock();
        // make_unique_and_ordered(queue_);
        // for (auto& [_, e] : queue_)
        //     System::post_event(e);
        for (auto& p : paints_)
            System::send_event(std::move(p));
        // paints_.clear();
        // queue_.clear();
        // running_count_ = 0;
    }

   private:
    std::set<Paint_event> paints_;
    // pull this out into a separate type so you can easily test.
    // Paint_event_container queue_;
    // int running_count_ = 0;

   private:
    /// modifies \p container to only have unique elements, ordered by count.
    // static void make_unique_and_ordered(Paint_event_container& container)
    // {
    //     // TODO
    //     // std::stable_sort(container) by address_of(receiver.get())
    //     // std::equal(reverse(container)) with equality by ^^ address of
    //     // erase the back half by using normal std::begin() and equal
    //     //  result.base(). This should erase the throw out section, but double
    //     //  check what reverse_iterator::base() exactly returns.
    //     // std::sort(container) by priority now
    //     // done
    // }

// on send_all() you sort the queue by address(Paint_event::receiver.get());
// Then you call unique() on the collection, but this doesn't work because it
// doesn't guarantee that you will save the one with the largest pair.
// It does work: "Eliminates all except the first element from every consecutive
// group of equivalent elements from the range" that is std::equal, so if you
// are appending in order received, and you stable_sort() and you do equal over
// reverse iterators, then it should all work. and erasing from unique end iter.
// Then forward iteration leaves you with a unique list of paint events and you
// can then sort them(unstable) by their count value(pair::first). and then
// forward iterate over them and send them all.
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
        return current + 1 < basics_.size() ? current + 1 : -1uL;
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

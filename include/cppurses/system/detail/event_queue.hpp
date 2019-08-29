#ifndef CPPURSES_SYSTEM_DETAIL_EVENT_QUEUE_HPP
#define CPPURSES_SYSTEM_DETAIL_EVENT_QUEUE_HPP
#include <algorithm>
#include <cstddef>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

#include <cppurses/system/event.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
namespace detail {

/// Holds Events to be processed, concurrent append
class Event_queue {
    using Mutex_t = std::mutex;
    using Guard_t = std::lock_guard<Event_queue::Mutex_t>;
    using Queue_t = std::vector<std::unique_ptr<Event>>;

    Queue_t general_events_;
    Queue_t paint_events_;
    Queue_t delete_events_;
    Mutex_t mtx_;

   public:
    /// Place \p event at the back of the queue.
    auto append(std::unique_ptr<Event> event) -> void
    {
        Guard_t g{mtx_};
        const auto type = event->type();
        if (type == Event::Paint)
            paint_events_.emplace_back(std::move(event));
        else if (type == Event::Delete)
            delete_events_.emplace_back(std::move(event));
        else
            general_events_.emplace_back(std::move(event));
    }

    /// Remove all nullptr Events.
    auto clean() -> void
    {
        Guard_t g{mtx_};
        remove_nulls(general_events_);
        remove_nulls(paint_events_);
        remove_nulls(delete_events_);
    }

    /// Remove all events that have a receiver of \p receiver from queue.
    /** To be called when sending delete events so that other threads may
     *  not crash the app by posting events to deleted Widgets.*/
    auto remove_events_of(Widget* receiver) -> void
    {
        Guard_t g{mtx_};
        remove_receiver(general_events_, receiver);
        remove_receiver(paint_events_, receiver);
        remove_descendants(general_events_, receiver);
        remove_descendants(paint_events_, receiver);
    }

    // Accessor Types ----------------------------------------------------------

    /// Provides iterator access to \p filter_ type elements in an Event_queue.
    /** filter: None - gives you all event types except Paint and Delete.
     *  This type is for exclusive use by Event_engine class, single thread. */
    template <Event::Type filter_type>
    class View {
        using Guard_t = Event_queue::Guard_t;
        using Size_t  = Event_queue::Queue_t::size_type;
        Event_queue& queue_;

       public:
        /// Construct a view over \p queue that will only access \p filter type.
        View(Event_queue& queue) : queue_{queue} {}

        /// Provides a forward iterator capable of moving events out of a view.
        class Move_iterator {
            using Size_t = View::Size_t;
            using Set_t  = std::set<Widget*>;
            Mutex_t& mtx_;
            Set_t already_sent_;
            Queue_t& events_;
            Size_t at_;

           public:
            /// Construct an iterator pointing to the first element in \p view.
            Move_iterator(Event_queue& queue)
                : mtx_{queue.mtx_},
                  events_{get_events(queue)},
                  at_{this->find_next(static_cast<Size_t>(-1))}
            {}

            /// Construct an end iterator.
            Move_iterator(Event_queue& queue, int)
                : mtx_{queue.mtx_}, events_{queue.general_events_}, at_{0}
            {}

            /// Move the currently pointed to Event object out of the queue.
            auto operator*() -> std::unique_ptr<Event> { return remove(at_); }

            /// Increment to the next element in queue that passes the filter.
            auto operator++() -> Move_iterator&
            {
                at_ = find_next(at_);
                return *this;
            }

            /// Returns whether or not this iterator is at the end of the queue.
            auto operator!=(const Move_iterator&) const -> bool
            {
                return at_ != this->size();
            }

           private:
            /// Retrieve the inner vector of events for the given event filter.
            static auto get_events(Event_queue& queue) -> Queue_t&
            {
                return queue.general_events_;
            }

            /// Return the next valid index after \p from for filter.
            auto find_next(Size_t from) -> Size_t
            {
                Guard_t g{mtx_};
                const auto end = events_.size();
                if (from == end)
                    return from;
                while (++from != end && events_[from] == nullptr) {}
                return from;
            }

            /// Remove and return the Event at \p at in events_.
            auto remove(Size_t at) -> std::unique_ptr<Event>
            {
                Guard_t g{mtx_};
                return std::move(events_[at]);
            }

            /// Retrieve the size of events_, including null-ed items.
            auto size() const -> Size_t
            {
                Guard_t g{mtx_};
                return events_.size();
            }
        };

        /// Return iterator the first element in queue.
        auto begin() -> Move_iterator { return Move_iterator{queue_}; }

        /// Return iterator to one past the last element in queue.
        auto end() -> Move_iterator { return Move_iterator{queue_, 0}; };
    };

   private:
    /// Remove all nullptrs from \p events queue.
    static auto remove_nulls(Queue_t& events) -> void
    {
        auto iter = std::find(std::rbegin(events), std::rend(events), nullptr);
        if (iter != std::rend(events))
            events.erase(std::begin(events), iter.base());
    }

    /// Remove any items from \p events that would be send to \p receiver.
    static auto remove_receiver(Queue_t& events, const Widget* receiver) -> void
    {
        for (auto& event : events) {
            if (event != nullptr && &(event->receiver()) == receiver)
                event.reset(nullptr);
        }
    }

    /// Remove from \p events where event.reciever() is an ancestor of receiver.
    static auto remove_descendants(Queue_t& events, const Widget* receiver)
        -> void
    {
        for (auto& event : events) {
            if (event != nullptr &&
                receiver->children.has_descendant(&(event->receiver()))) {
                event.reset(nullptr);
            }
        }
    }
};

template <>
inline auto Event_queue::View<Event::Paint>::Move_iterator::get_events(
    Event_queue& queue) -> Queue_t&
{
    return queue.paint_events_;
}

template <>
inline auto Event_queue::View<Event::Delete>::Move_iterator::get_events(
    Event_queue& queue) -> Queue_t&
{
    return queue.delete_events_;
}

template <>
inline auto Event_queue::View<Event::Paint>::Move_iterator::find_next(
    Size_t from) -> Size_t
{
    Guard_t g{mtx_};
    const auto end = events_.size();
    if (from == end)
        return from;
    while (++from != end) {
        if (events_[from] == nullptr)
            continue;
        if (already_sent_.count(&(events_[from]->receiver())) > 0) {
            events_[from].reset(nullptr);
            continue;
        }
        already_sent_.insert(&(events_[from]->receiver()));
        break;
    }
    return from;
}

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_EVENT_QUEUE_HPP

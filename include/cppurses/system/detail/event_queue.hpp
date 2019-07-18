#ifndef CPPURSES_SYSTEM_DETAIL_EVENT_QUEUE_HPP
#define CPPURSES_SYSTEM_DETAIL_EVENT_QUEUE_HPP
#include <algorithm>
#include <cstddef>
#include <memory>
#include <mutex>
#include <vector>

#include <cppurses/system/event.hpp>

namespace cppurses {
class Event;
namespace detail {

/// Holds Events to be processed, concurrent append
class Event_queue {
    using Mutex_t = std::mutex;
    using Queue_t = std::vector<std::unique_ptr<Event>>;
    using Guard_t = std::lock_guard<Event_queue::Mutex_t>;

    Queue_t events_;
    Mutex_t mtx_;

   public:
    /// Place \p event at the back of the queue.
    auto append(std::unique_ptr<Event> event) -> void
    {
        Guard_t g{mtx_};
        if (is_expensive(event->type()))
            this->remove_duplicates(*event);
        events_.emplace_back(std::move(event));
    }

    // Accessor Types ----------------------------------------------------------

    /// Provides iterator access to \p filter type elements in an Event_queue.
    /** filter: None - gives you all event types except Paint and Delete.
     *  This type is for exclusive use by Event_engine class, single thread. */
    class Filter_view {
        using Guard_t = Event_queue::Guard_t;
        using Size_t  = Event_queue::Queue_t::size_type;
        Event_queue& queue_;
        Event::Type filter_;

       public:
        /// Construct a view over \p queue that will only access \p filter type.
        Filter_view(Event_queue& queue, Event::Type filter)
            : queue_{queue}, filter_{filter}
        {}

        /// Provides a forward iterator capable of moving events out of a view.
        class Move_iterator {
            using Size_t = Filter_view::Size_t;
            Filter_view* const view_{nullptr};
            Size_t at_;

           public:
            /// Construct an end iterator.
            Move_iterator() = default;

            /// Construct an iterator pointing to the first element in \p view.
            Move_iterator(Filter_view& view)
                : view_{&view}, at_{view_->find_next(-1)}
            {}

            /// Move the currently pointed to Event object out of the queue.
            auto operator*() -> std::unique_ptr<Event>
            {
                return view_->remove(at_);
            }

            /// Increment to the next element in queue that passes the filter.
            auto operator++() -> Move_iterator&
            {
                at_ = view_->find_next(at_);
                return *this;
            }

            /// Returns whether or not this iterator is at the end of the queue.
            auto operator!=(const Move_iterator&) const -> bool
            {
                return at_ != view_->size();
            }
        };

        /// Return iterator the first element in queue.
        auto begin() -> Move_iterator { return Move_iterator{*this}; }

        /// Return iterator to one past the last element in queue.
        auto end() -> Move_iterator { return Move_iterator{}; };

        /// Remove all nullptr Events.
        auto cleanup() -> void
        {
            Guard_t g{queue_.mtx_};
            auto& events = queue_.events_;
            events.erase(
                std::remove(std::begin(events), std::end(events), nullptr),
                std::cend(events));
        }

        /// Remove all events that have a receiver of \p receiver from queue.
        /** To be called when sending delete events so that other threads may
         *  not crash the app by posting events to deleted Widgets.*/
        // TODO Should this also remove events to descendants?
        auto remove_events_of(Widget* receiver) -> void
        {
            Guard_t g{queue_.mtx_};
            for (auto& event : queue_.events_) {
                if (event != nullptr && &(event->receiver()) == receiver)
                    event.reset(nullptr);
            }
        }

       private:
        // Implementation functions for Move_iterator.
        /// Return the next valid index after \p from for filter.
        auto find_next(Size_t from) -> Size_t
        {
            Guard_t g{queue_.mtx_};
            const auto end = queue_.events_.size();
            if (from == end)
                return from;
            ++from;
            while (from != end) {
                auto& event = queue_.events_[from];
                if (event == nullptr)
                    ++from;
                else if (event->type() == filter_ ||
                         (event->type() != Event::Delete &&
                          event->type() != Event::Paint &&
                          filter_ == Event::None)) {
                    break;
                }
                else {
                    ++from;
                }
            }
            return from;
        }

        /// Remove and return the Event at \p at in queue_.events_.
        auto remove(Size_t at) -> std::unique_ptr<Event>
        {
            Guard_t g{queue_.mtx_};
            return std::move(queue_.events_[at]);
        }

        auto size() const -> Size_t
        {
            Guard_t g{queue_.mtx_};
            return queue_.events_.size();
        }
    };

   private:
    /// Determines the event types to remove duplicates of.
    static auto is_expensive(Event::Type type) -> bool
    {
        switch (type) {
            case Event::Paint:
            case Event::Move:
            case Event::Resize:
            case Event::Disable:
            case Event::Enable: return true;
            default: return false;
        }
    }

    /// Sets the first event in events_ that is equal to \p event to nullptr.
    auto remove_duplicates(const Event& event) -> void
    {
        for (auto& queued : events_) {
            if (queued != nullptr && *queued == event) {
                queued = nullptr;
                return;
            }
        }
    }
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_EVENT_QUEUE_HPP

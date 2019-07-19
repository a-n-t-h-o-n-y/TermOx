#ifndef CPPURSES_SYSTEM_DETAIL_EVENT_ENGINE_HPP
#define CPPURSES_SYSTEM_DETAIL_EVENT_ENGINE_HPP
#include <atomic>

#include <cppurses/painter/detail/screen.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/system/detail/event_queue.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/system.hpp>

namespace cppurses {
namespace detail {

/// Simplified std::condition_variable
// class Condition {
//     std::mutex mtx_;
//     std::condition_variable cv_;
//     bool is_ready_{false};

//    public:
//     /// Blocks until another thread calls notify().
//     auto wait() -> void
//     {
//         std::unique_lock<std::mutex> lock{mtx_};
//         cv_.wait(lock, [this] { return is_ready_; });
//         is_ready_ = false;
//     }

//     /// Wakes up a blocking call to wait().
//     auto notify() -> void
//     {
//         std::unique_lock<std::mutex> lock{mtx_};
//         is_ready_ = true;
//         lock.unlock();
//         cv_.notify_one();
//     }
// };

/// Orchestrates all event processing and queueing.
class Event_engine {
    Event_queue queue_;
    std::atomic<bool> notified_{false};  // Whether should process events/flush

   public:
    /// Event_loop uses this to let main thread know it should process events.
    auto notify() -> void { notified_ = true; }

    /// Invokes events and flush the screen.
    auto process() -> void
    {
        invoke_events(queue_);
        flush_screen();
    }

    /// Return a reference to the internal Event_queue.
    auto queue() -> Event_queue& { return queue_; }

    /// Get the global Event_engine instantiation.
    static auto get() -> Event_engine&
    {
        static Event_engine engine;
        return engine;
    }

   private:
    Event_engine() = default;

    /// Flushes all of the staged changes to the screen and sets the cursor.
    static auto flush_screen() -> void
    {
        auto& staged_changes = Staged_changes::get();
        Screen::flush(staged_changes);
        staged_changes.clear();
        Screen::set_cursor_on_focus_widget();
    }

    /// Send all \p type events in queue to their Widgets.
    static auto send_all(Event_queue& queue, Event::Type type) -> void
    {
        using Event_ptr = std::unique_ptr<Event>;
        for (Event_ptr event : Event_queue::Filter_view{queue, type}) {
            System::send_event(*event);
        }
    }

    /// Send all delete events to their Widgets.
    /** Removes any events to the receiver if another thread posted them. */
    static auto send_all_deletes(Event_queue& queue) -> void
    {
        auto view = Event_queue::Filter_view{queue, Event::Delete};
        for (std::unique_ptr<Event> event : view) {
            Widget* receiver = &(event->receiver());
            System::send_event(*event);
            view.remove_events_of(receiver);
        }
    }

    /// Remove all sent events from the queue.
    static auto clean(Event_queue& queue) -> void
    {
        auto view = Event_queue::Filter_view{queue, Event::None};
        view.cleanup();
    }

    /// Sends each event in \p queue to its receiver to be processed.
    static auto invoke_events(Event_queue& queue) -> void
    {
        send_all(queue, Event::None);
        send_all(queue, Event::Paint);
        send_all_deletes(queue);
        clean(queue);
    }
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_EVENT_ENGINE_HPP

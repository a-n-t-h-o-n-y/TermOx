#ifndef CPPURSES_SYSTEM_DETAIL_EVENT_ENGINE_HPP
#define CPPURSES_SYSTEM_DETAIL_EVENT_ENGINE_HPP
#include <condition_variable>
#include <future>
#include <mutex>

#include <fstream> //temp

#include <cppurses/painter/detail/screen.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/system/detail/event_queue.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/terminal/ncurses_mutex.hpp>

namespace cppurses {
namespace detail {

/// Simplified std::condition_variable
class Condition {
    std::mutex mtx_;
    std::condition_variable cv_;
    bool is_ready_{false};

   public:
    /// Blocks until another thread calls notify().
    auto wait() -> void
    {
        std::unique_lock<std::mutex> lock{mtx_};
        cv_.wait(lock, [this] { return is_ready_; });
        is_ready_ = false;
    }

    /// Wakes up a blocking call to wait().
    auto notify() -> void
    {
        std::unique_lock<std::mutex> lock{mtx_};
        is_ready_ = true;
        lock.unlock();
        cv_.notify_one();
    }
};

/// Orchestrates all event processing and queueing.
class Event_engine {
    Event_queue queue_;
    Condition condition_;
    bool exit_{false};
    std::future<void> fut_;

   public:
    ~Event_engine() { this->exit(); }

    /// Launch a new thread that will process events on process_queue() calls.
    /** exit() will stop processing and close this thread. */
    auto launch() -> void
    {
        exit_ = false;
        fut_  = std::async(std::launch::async, [this]() { this->process(); });
    }

    auto exit() -> void
    {
        exit_ = true;
        this->process_queue();  // break out of loop
    }

    /// Wakes async thread to process each event, used by Event_loop.
    auto process_queue() -> void { condition_.notify(); }

    /// Return a reference to the Event_queue.
    auto queue() -> Event_queue& { return queue_; }

    // So event loops can process the queue and flush.
    static auto get() -> Event_engine&
    {
        static Event_engine engine;
        return engine;
    }

   private:
    Event_engine() = default;

    auto process() -> void
    {
        while (true) {
            condition_.wait();
            if (exit_)
                break;
            invoke_events(queue_);
            flush_screen();
        }
        exit_ = false;
    }

    /// Flushes all of the staged changes to the screen and sets the cursor.
    static auto flush_screen() -> void
    {
        auto& staged_changes = Staged_changes::get();
        {
            std::ofstream l{"log", std::ios::app};
            l << "locking Event_engine::flush_screen()" << std::endl;
            std::lock_guard<std::mutex> lock{ncurses_mutex()};
            Screen::flush(staged_changes);
            l << "unlocking Event_engine::flush_screen()" << std::endl;
        }
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

    /// Sends each event in \p queue to its receiver to be processed.
    static auto invoke_events(Event_queue& queue) -> void
    {
        send_all(queue, Event::None);
        send_all(queue, Event::Paint);
        // TODO last system flushed here and then sent delete events.
        send_all_deletes(queue);
    }
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_EVENT_ENGINE_HPP

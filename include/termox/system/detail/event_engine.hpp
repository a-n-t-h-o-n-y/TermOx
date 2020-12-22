#ifndef TERMOX_SYSTEM_DETAIL_EVENT_ENGINE_HPP
#define TERMOX_SYSTEM_DETAIL_EVENT_ENGINE_HPP
#include <memory>

#include <termox/painter/detail/screen.hpp>
#include <termox/painter/detail/staged_changes.hpp>
#include <termox/system/detail/event_queue.hpp>
#include <termox/system/event.hpp>
#include <termox/system/system.hpp>

namespace ox::detail {

/// Orchestrates all event processing and queueing.
class Event_engine {
   public:
    /// Invokes events and flush the screen.
    void process()
    {
        queue_.send_all();
        flush_screen();
    }

    /// Return a reference to the internal Event_queue.
    auto queue() -> Event_queue& { return queue_; }

   private:
    /// Flushes all of the staged changes to the screen and sets the cursor.
    static void flush_screen()
    {
        auto& staged_changes = Staged_changes::get();
        Screen::flush(staged_changes);
        staged_changes.clear();
        Screen::display_cursor();
    }

   private:
    Event_queue queue_;
};

}  // namespace ox::detail
#endif  // TERMOX_SYSTEM_DETAIL_EVENT_ENGINE_HPP

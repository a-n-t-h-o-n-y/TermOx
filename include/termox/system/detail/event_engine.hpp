#ifndef TERMOX_SYSTEM_DETAIL_EVENT_ENGINE_HPP
#define TERMOX_SYSTEM_DETAIL_EVENT_ENGINE_HPP
#include <cassert>
#include <memory>

#include <termox/painter/detail/is_paintable.hpp>
#include <termox/painter/detail/screen.hpp>
#include <termox/painter/detail/staged_changes.hpp>
#include <termox/system/detail/event_queue.hpp>
#include <termox/system/event.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/point.hpp>

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
        System::terminal.refresh();
        // Cursor
        Widget const* const fw = System::focus_widget();
        if (fw != nullptr && is_paintable(*fw)) {
            assert(is_within(fw->cursor.position(), fw->area()));
            System::set_cursor(fw->cursor, fw->top_left());
        }
    }

    /// Return true if \p point is within \p area.
    [[nodiscard]] static auto is_within(Point point, Area area) -> bool
    {
        return point.x < area.width && point.y < area.height;
    }

   private:
    Event_queue queue_;
};

}  // namespace ox::detail
#endif  // TERMOX_SYSTEM_DETAIL_EVENT_ENGINE_HPP

#ifndef CPPURSES_SYSTEM_DETAIL_EVENT_QUEUE_HPP
#define CPPURSES_SYSTEM_DETAIL_EVENT_QUEUE_HPP
#include <memory>
#include <vector>

namespace cppurses {
class Event;
namespace detail {

/// Holds Events to be invoked at a later time.
/** Events are added to an Event_queue by calling System::post_event(). Each
 *  Event_loop holds its own Event_queue, System::post_event() finds this
 *  Event_queue by the calling std::thread::id. */
class Event_queue {
   public:
    /// Moves \p event onto the Event_queue for later processing.
    /** This function implements optimizations on a few Event::Types. In these
     *  cases any previous Events of the same Event::Type and with the same
     *  receiver will be removed and the new \p event inserted at the end of the
     *  Event_queue. */
    void append(std::unique_ptr<Event> event);

    friend class Event_invoker;

   private:
    std::vector<std::unique_ptr<Event>> queue_;
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_EVENT_QUEUE_HPP

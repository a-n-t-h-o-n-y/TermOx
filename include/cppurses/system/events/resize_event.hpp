#ifndef SYSTEM_EVENTS_RESIZE_EVENT_HPP
#define SYSTEM_EVENTS_RESIZE_EVENT_HPP
#include <cppurses/system/event.hpp>

#include <cstddef>

namespace cppurses {
class Event_handler;

class Resize_event : public Event {
   public:
    Resize_event(Event_handler* receiver,
                 std::size_t new_width,
                 std::size_t new_height,
                 std::size_t old_width = 0,
                 std::size_t old_height = 0);

    bool send() const override;
    bool filter_send(Event_handler* filter) const override;

   protected:
    std::size_t new_width_;
    std::size_t new_height_;
    std::size_t old_width_;
    std::size_t old_height_;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_RESIZE_EVENT_HPP

#ifndef CPPURSES_SYSTEM_EVENTS_FOCUS_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_FOCUS_EVENT_HPP
#include <cppurses/system/event.hpp>

namespace cppurses {

/// Sent to Widget that have just become the Widget in focus.
class Focus_in_event : public Event {
   public:
    bool send() const override;
    bool filter_send(Widget* filter) const override;
    friend class Focus;

   private:
    explicit Focus_in_event(Widget* receiver);
};

/// Sent to Widget that have just lost the focus.
class Focus_out_event : public Event {
   public:
    bool send() const override;
    bool filter_send(Widget* filter) const override;
    friend class Focus;

   private:
    explicit Focus_out_event(Widget* receiver);
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_FOCUS_EVENT_HPP

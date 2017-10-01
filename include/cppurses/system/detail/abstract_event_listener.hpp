#ifndef SYSTEM_DETAIL_ABSTRACT_EVENT_LISTENER_HPP
#define SYSTEM_DETAIL_ABSTRACT_EVENT_LISTENER_HPP
#include <memory>

namespace cppurses {
class Event;
namespace detail {

class Abstract_event_listener {
   public:
    virtual std::unique_ptr<Event> get_input() const = 0;
    virtual void enable_ctrl_characters() = 0;
    virtual void disable_ctrl_characters() = 0;
};

}  // namespace detail
}  // namespace cppurses
#endif  // SYSTEM_DETAIL_ABSTRACT_EVENT_LISTENER_HPP

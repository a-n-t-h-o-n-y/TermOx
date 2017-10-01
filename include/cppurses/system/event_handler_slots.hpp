#ifndef SYSTEM_EVENT_HANDLER_SLOTS_HPP
#define SYSTEM_EVENT_HANDLER_SLOTS_HPP
#include <signals/slot.hpp>

namespace cppurses {
class Event_handler;
namespace slot {

sig::Slot<void()> enable(Event_handler& eh);

sig::Slot<void()> disable(Event_handler& eh);

}  // namespace slot
}  // namespace cppurses

#endif  // SYSTEM_EVENT_HANDLER_SLOTS_HPP

#include <cppurses/system/event_handler_slots.hpp>

#include <signals/slot.hpp>

#include <cppurses/system/event_handler.hpp>
#include <cppurses/system/events/disable_event.hpp>
#include <cppurses/system/events/enable_event.hpp>
#include <cppurses/system/system.hpp>

namespace cppurses {
namespace slot {

sig::Slot<void()> enable(Event_handler& eh) {
    sig::Slot<void()> slot{[&eh] { System::post_event<Enable_event>(&eh); }};
    slot.track(eh.destroyed);
    return slot;
}

sig::Slot<void()> disable(Event_handler& eh) {
    sig::Slot<void()> slot{[&eh] { System::post_event<Disable_event>(&eh); }};
    slot.track(eh.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace cppurses

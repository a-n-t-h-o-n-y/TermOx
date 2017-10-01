#include "system/event_handler_slots.hpp"
#include "system/event_handler.hpp"
#include <signals/slot.hpp>

namespace cppurses {
namespace slot {

sig::Slot<void()> enable(Event_handler& eh) {
    sig::Slot<void()> slot{[&eh] { eh.set_enabled(true); }};
    slot.track(eh.destroyed);
    return slot;
}

sig::Slot<void()> disable(Event_handler& eh) {
    sig::Slot<void()> slot{[&eh] { eh.set_enabled(false); }};
    slot.track(eh.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace cppurses

#include <termox/system/event_loop.hpp>

#include <termox/system/event_queue.hpp>
#include <termox/system/system.hpp>

namespace ox {

void Event_loop::connect_to_system_exit()
{
    auto exit_loop =
        sl::Slot<void(int)>{[this](int code) { this->exit(code); }};
    exit_loop.track(lifetime_);
    System::exit_signal.connect(exit_loop);
}

// Required here because event_queue.hpp can't be in header, but run is template
void Event_loop::send_all_events_and_flush() { Event_queue::send_all(); }

}  // namespace ox

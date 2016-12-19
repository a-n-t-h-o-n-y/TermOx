#include <system_module/abstract_event_dispatcher.hpp>
#include <system_module/event.hpp>
#include <system_module/object.hpp>
#include <system_module/system.hpp>

#include <memory>

namespace mcurses {

bool Abstract_event_dispatcher::process_events() {
    interrupt_ = false;
    System::send_posted_events();
    if (!interrupt_) {
        System::send_posted_events(nullptr, Event::Type::DeferredDelete);
    }
    if (!interrupt_) {
        this->post_user_input();  // Blocking call
    }
    return true;  // return true if an event was processed, doesn't seem to be
                  // used anywhere
}

// Interrupt the dispatching of events if Event_loop::exit() is called
void Abstract_event_dispatcher::interrupt() {
    interrupt_ = true;
}

}  // namespace mcurses

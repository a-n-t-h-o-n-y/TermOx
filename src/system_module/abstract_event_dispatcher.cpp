#include <system_module/abstract_event_dispatcher.hpp>
#include <system_module/event.hpp>
#include <system_module/object.hpp>
#include <system_module/system.hpp>

#include <memory>

namespace twf {

bool Abstract_event_dispatcher::process_events() {
    interrupt_ = false;
    System::send_posted_events();
    if (!interrupt_) {
        System::send_posted_events(nullptr, Event::DeferredDelete);
    }
    if (!interrupt_) {
        // System::paint_engine()->flush(true); // this is now in
        // send_posted_events, but that is twice as many calls per loop now,
        // work out something better for this whole event loop structure, in
        // regards to processing DeferredDelete events. You can put the
        // processing for deferred delete at the end of send_posted_events()
    }
    if (!interrupt_) {
        this->post_user_input();  // Blocking call
    }
    return !interrupt_;
}

// Interrupt the dispatching of events if Event_loop::exit() is called
void Abstract_event_dispatcher::interrupt() {
    interrupt_ = true;
}

}  // namespace twf

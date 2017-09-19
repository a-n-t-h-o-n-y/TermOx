#include "system/abstract_event_dispatcher.hpp"
#include "system/event.hpp"
#include "system/system.hpp"

namespace cppurses {

// bool Abstract_event_dispatcher::process_events() {
//     interrupt_ = false;
//     System::send_posted_events();
//     if (!interrupt_) {
//         System::send_posted_events(nullptr, Event::DeferredDelete);
//     }
//     if (!interrupt_) {
//         System::paint_engine()->flush(true);
//     }
//     if (!interrupt_) {
//         this->post_user_input();  // Blocking call
//     }
//     return !interrupt_;
// }

// Interrupt the dispatching of events if Event_loop::exit() is called
// void Abstract_event_dispatcher::interrupt() {
//     interrupt_ = true;
// }

}  // namespace cppurses

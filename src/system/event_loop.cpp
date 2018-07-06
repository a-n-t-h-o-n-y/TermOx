#include <cppurses/system/event_loop.hpp>

#include <utility>

#include <cppurses/painter/paint_buffer.hpp>
#include <cppurses/system/detail/abstract_event_listener.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/system.hpp>

namespace cppurses {

int Event_loop::run() {
    exit_ = false;
    while (!exit_) {
        this->process_events();
    }
    return return_code_;
}

void Event_loop::exit(int return_code) {
    return_code_ = return_code;
    exit_ = true;
}

void Event_loop::process_events() {
    invoker_.invoke(event_queue);
    if (!exit_) {
        invoker_.invoke(event_queue, Event::DeferredDelete);
        if (System::paint_buffer() != nullptr) {
            System::paint_buffer()->flush(true);
        }
        // Blocking Call
        auto event_ptr = System::event_listener()->get_input();
        event_queue.append(std::move(event_ptr));
    }
}

}  // namespace cppurses

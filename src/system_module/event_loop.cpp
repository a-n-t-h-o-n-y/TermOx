#include <painter_module/paint_engine.hpp>
#include <system_module/detail/thread_data.hpp>
#include <system_module/event_loop.hpp>
// #include <system_module/system.hpp>

namespace mcurses {

int Event_loop::run() {
    auto& data = detail::Thread_data::current();
    data.event_loops.push(this);

    while (!exit_) {
        this->process_events();
    }

    data.event_loops.pop();

    return return_code_;
}

void Event_loop::exit(int return_code) {
    return_code_ = return_code;
    exit_ = true;
    detail::Thread_data::current().dispatcher().interrupt();
}

bool Event_loop::process_events() {
    auto& data = detail::Thread_data::current();
    return data.dispatcher().process_events();
}

}  // namespace mcurses

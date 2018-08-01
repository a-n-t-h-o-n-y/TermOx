#include <cppurses/system/event_loop.hpp>

#include <mutex>
#include <thread>
#include <utility>

// #include <cppurses/painter/detail/flush.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/painter/paint_buffer.hpp>
#include <cppurses/system/detail/abstract_event_listener.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/system.hpp>

namespace cppurses {

Event_loop::Event_loop(const std::function<void()>& loop_function)
    : loop_func_{loop_function} {}

int Event_loop::run() {
    if (running_) {
        return -1;
    }
    running_ = true;
    thread_id_ = std::this_thread::get_id();
    exit_ = false;
    while (!exit_) {
        this->process_events();
    }
    running_ = false;
    return return_code_;
}

void Event_loop::exit(int return_code) {
    return_code_ = return_code;
    exit_ = true;
}

void Event_loop::process_events() {
    static std::mutex mtx;
    mtx.lock();
    invoker_.invoke(event_queue);
    if (!exit_) {
        // TODO move below call to invoke to after call to flush()?
        invoker_.invoke(event_queue, Event::DeferredDelete);
        System::paint_buffer().flush(staged_changes_);
        mtx.unlock();
        staged_changes_.clear();
        loop_func_();
    } else {
        mtx.unlock();
    }
}

std::thread::id Event_loop::get_thread_id() const {
    return thread_id_;
}

const detail::Staged_changes& Event_loop::staged_changes() const {
    return staged_changes_;
}

detail::Staged_changes& Event_loop::staged_changes() {
    return staged_changes_;
}

}  // namespace cppurses

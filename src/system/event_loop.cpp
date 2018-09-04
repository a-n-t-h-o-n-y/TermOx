#include <cppurses/system/event_loop.hpp>

#include <future>
#include <mutex>
#include <thread>
#include <utility>

#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/system/detail/event_invoker.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/system.hpp>

namespace cppurses {

// You can remove these if you replace the std::atomic<bool> exit_ member.
Event_loop::Event_loop(Event_loop&& other)
    : fut_{std::move(other.fut_)},
      thread_id_{std::move(other.thread_id_)},
      return_code_{std::move(other.return_code_)},
      exit_{other.exit_.load()},
      running_{std::move(other.running_)},
      event_queue_{std::move(other.event_queue_)},
      invoker_{std::move(other.invoker_)},
      staged_changes_{std::move(other.staged_changes_)},
      paint_middleman_{std::move(other.paint_middleman_)} {}

Event_loop& Event_loop::operator=(Event_loop&& other) {
    if (this != &other) {
        fut_ = std::move(other.fut_);
        thread_id_ = std::move(other.thread_id_);
        return_code_ = std::move(other.return_code_);
        exit_ = other.exit_.load();
        running_ = std::move(other.running_);
        event_queue_ = std::move(other.event_queue_);
        invoker_ = std::move(other.invoker_);
        staged_changes_ = std::move(other.staged_changes_);
        paint_middleman_ = std::move(other.paint_middleman_);
    }
    return *this;
}

Event_loop::~Event_loop() {
    this->exit(0);
    this->wait();
}

int Event_loop::run() {
    if (running_) {
        return -1;
    }
    exit_ = false;
    running_ = true;
    thread_id_ = std::this_thread::get_id();
    System::register_event_loop(this);
    while (!exit_) {
        this->process_events();
    }
    running_ = false;
    System::deregister_event_loop(this);
    return return_code_;
}

void Event_loop::run_async() {
    fut_ = std::async(std::launch::async, [this] { return this->run(); });
}

int Event_loop::wait() {
    if (fut_.valid()) {
        return fut_.get();
    }
    return -1;
}

void Event_loop::exit(int return_code) {
    return_code_ = return_code;
    exit_ = true;
}

void Event_loop::process_events() {
    // only one event loop at a time can be invoking its queue.
    static std::mutex mtx;
    mtx.lock();
    invoker_.invoke(event_queue_);
    if (!exit_) {
        invoker_.invoke(event_queue_, Event::Paint);
        paint_middleman_.flush(staged_changes_);
        paint_middleman_.set_cursor_on_focus_widget();
        staged_changes_.clear();
        invoker_.invoke(event_queue_, Event::Delete);
        mtx.unlock();
        this->loop_function();
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

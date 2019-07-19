#include <cppurses/system/event_loop.hpp>

#include <future>
#include <thread>
#include <utility>

#include <cppurses/system/detail/event_engine.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/system.hpp>

namespace cppurses {

auto Event_loop::run() -> int
{
    if (running_)
        return -1;
    exit_       = false;
    running_    = true;
    auto notify = true;
    while (!exit_) {
        if (notify)
            detail::Event_engine::get().notify();
        if (is_main_thread_)
            detail::Event_engine::get().process();
        notify = this->loop_function();
    }
    running_ = false;
    return return_code_;
}

auto Event_loop::run_async() -> void
{
    fut_ = std::async(std::launch::async, [this] { return this->run(); });
}

auto Event_loop::wait() -> int
{
    if (fut_.valid())
        return fut_.get();
    return -1;
}

// void Event_loop::process_events()
// {
//     // only one event loop at a time can be invoking its queue.
//     static std::mutex mtx;
//     mtx.lock();
//     invoker_.invoke(event_queue_);
//     if (!exit_) {
//         invoker_.invoke(event_queue_, Event::Paint);
//         screen_.flush(staged_changes_);
//         screen_.set_cursor_on_focus_widget();
//         staged_changes_.clear();
//         invoker_.invoke(event_queue_, Event::Delete);
//         mtx.unlock();
//         this->loop_function();
//     }
//     else {
//         mtx.unlock();
//     }
// }

auto Event_loop::connect_to_system_exit() -> void
{
    sig::Slot<void(int)> exit_loop{[this](int code) { this->exit(code); }};
    exit_loop.track(lifetime);
    System::exit_signal.connect(exit_loop);
}
}  // namespace cppurses

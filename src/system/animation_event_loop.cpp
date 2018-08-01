#include <cppurses/system/detail/animation_event_loop.hpp>

#include <chrono>
#include <future>
#include <memory>
#include <thread>

#include <signals/signals.hpp>

#include <cppurses/system/events/animation_event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
namespace slot {
sig::Slot<void()> post_animation_event(Widget* w_ptr) {
    sig::Slot<void()> slot{
        [w_ptr] { System::post_event<Animation_event>(w_ptr); }};
    slot.track(w_ptr->destroyed);
    return slot;
}
}  // namespace slot
namespace detail {

Animation_event_loop::Animation_event_loop(
    std::function<Period_t()> period_func)
    : Event_loop{[this] { this->post_all(); }}, period_func_{period_func} {}

Animation_event_loop::Animation_event_loop(Period_t period)
    : Animation_event_loop{[period] { return period; }} {}

Animation_event_loop::~Animation_event_loop() {
    this->exit(0);
    this->wait();
}

void Animation_event_loop::run() {
    fut_ = std::async(std::launch::async,
                      [this] { return this->Event_loop::run(); });
}

int Animation_event_loop::wait() {
    if (fut_.valid()) {
        fut_.wait();
        return fut_.get();
    }
    return -1;
}

void Animation_event_loop::register_widget(Widget& w) {
    if (connections_.count(&w) == 1) {
        return;
    }
    sig::Connection c{on_loop_.connect(slot::post_animation_event(&w))};
    connections_[&w] = c;
}

bool Animation_event_loop::unregister_widget(Widget& w) {
    auto iter = connections_.find(&w);
    if (iter != std::end(connections_)) {
        iter->second.disconnect();
        connections_.erase(iter);
        return true;
    }
    return false;
}

void Animation_event_loop::set_period(Period_t period) {
    period_func_ = [period] { return period; };
}

void Animation_event_loop::set_period(
    const std::function<Period_t()>& period_function) {
    period_func_ = period_function;
}

bool Animation_event_loop::empty() const {
    return connections_.empty();
}

void Animation_event_loop::post_all() {
    on_loop_();
    auto now = std::chrono::high_resolution_clock::now();
    auto time_passed = now - last_time_;
    auto time_to_sleep = period_func_() - time_passed;
    if (time_to_sleep > Period_t::zero()) {
        std::this_thread::sleep_for(time_to_sleep);
    }
    last_time_ = std::chrono::high_resolution_clock::now();
}

}  // namespace detail
}  // namespace cppurses

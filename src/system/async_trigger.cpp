#include <cppurses/system/async_trigger.hpp>

namespace cppurses {

Async_trigger::Async_trigger(Duration_t period)
    : func_{[period] { return period; }} {}

Async_trigger::Async_trigger(const std::function<Duration_t()>& period_function)
    : func_{period_function} {}

Async_trigger::Async_trigger(Async_trigger&& other) {
    std::lock_guard<std::mutex> guard(other.func_mutex_);
    trigger = std::move(other.trigger);
    func_ = std::move(other.func_);
    looping_ = other.looping_;
    if (this->looping_) {
        this->start();
    }
}

Async_trigger& Async_trigger::operator=(Async_trigger&& other) {
    if (&other != this) {
        std::unique_lock<std::mutex> lhs_lock{this->func_mutex_,
                                              std::defer_lock};
        std::unique_lock<std::mutex> rhs_lock{other.func_mutex_,
                                              std::defer_lock};
        std::lock(lhs_lock, rhs_lock);
        this->stop();
        trigger = std::move(other.trigger);
        func_ = std::move(other.func_);
        looping_ = other.looping_;
        if (this->looping_) {
            this->start();
        }
    }
    return *this;
}

Async_trigger::~Async_trigger() {
    this->stop();
}

void Async_trigger::start() {
    if (looping_) {
        return;
    }
    looping_ = true;
    future_ = std::async([this] { this->loop(); });
}

void Async_trigger::stop() {
    looping_ = false;
}

void Async_trigger::set_period(Duration_t period) {
    std::lock_guard<std::mutex> guard{func_mutex_};
    func_ = [period] { return period; };
}

void Async_trigger::set_period(
    const std::function<Duration_t()>& period_function) {
    std::lock_guard<std::mutex> guard{func_mutex_};
    func_ = period_function;
}

typename Async_trigger::Duration_t Async_trigger::get_period() const {
    std::lock_guard<std::mutex> guard{func_mutex_};
    return func_();
}

std::function<typename Async_trigger::Duration_t()>&
Async_trigger::get_period_function() {
    return func_;
}

std::function<typename Async_trigger::Duration_t()>
Async_trigger::get_period_function() const {
    std::lock_guard<std::mutex> guard{func_mutex_};
    return func_;
}

void Async_trigger::wait() {
    future_.wait();
}

void Async_trigger::loop() {
    while (looping_) {
        auto begin = std::chrono::high_resolution_clock::now();
        Duration_t period = func_();
        func_mutex_.lock();
        trigger();
        func_mutex_.unlock();
        auto end = std::chrono::high_resolution_clock::now();
        auto trigger_duration = end - begin;
        auto time_left = period - trigger_duration;
        if (time_left > time_left.zero()) {
            std::this_thread::sleep_for(time_left);
        }
    }
}

}  // namespace cppurses

#include <ox/timer.hpp>

#include <chrono>
#include <utility>

#include <zzz/timer_thread.hpp>

#include <ox/application.hpp>
#include <ox/core/terminal.hpp>
#include <ox/widget.hpp>

namespace ox {

Timer::Timer(Widget& w, std::chrono::milliseconds duration, bool launch)
    : id_{next_id_++}, duration_{duration}, timer_thread_{}
{
    Application::timer_targets.emplace(std::pair{id_, track(w)});
    if (launch) { this->start(); }
}

Timer::Timer(Timer&& other)
{
    id_ = std::move(other.id_);
    duration_ = std::move(other.duration_);
    timer_thread_ = std::move(other.timer_thread_);
    is_running_ = std::move(other.is_running_);
    other.is_running_ = false;
}

auto Timer::operator=(Timer&& other) -> Timer&
{
    if (this->is_running_) { this->stop(); }
    id_ = std::move(other.id_);
    duration_ = std::move(other.duration_);
    timer_thread_ = std::move(other.timer_thread_);
    is_running_ = std::move(other.is_running_);
    other.is_running_ = false;
    return *this;
}

Timer::~Timer()
{
    if (is_running_) { this->stop(); }
    Application::timer_targets.erase(id_);
}

void Timer::start()
{
    is_running_ = true;
    timer_thread_ = zzz::TimerThread{
        duration_,
        [id = id_] { Terminal::event_queue.enqueue(event::Timer{id}); },
    };
}

void Timer::stop()
{
    timer_thread_.request_stop();
    is_running_ = false;
}

}  // namespace ox
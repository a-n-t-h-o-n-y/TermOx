#include <termox/common/timer.hpp>

#include <algorithm>
#include <chrono>
#include <thread>

namespace ox {

void Timer::begin() { last_time_ = Clock_t::now(); }

void Timer::wait() { std::this_thread::sleep_for(this->get_sleep_time()); }

void Timer::set_interval(Duration_t interval) { interval_ = interval; }

auto Timer::get_interval() const -> Duration_t { return interval_; }

auto Timer::get_sleep_time() const -> Clock_t::duration
{
    auto const elapsed = Clock_t::now() - last_time_;
    return std::max(Clock_t::duration::zero(), interval_ - elapsed);
}

}  // namespace ox

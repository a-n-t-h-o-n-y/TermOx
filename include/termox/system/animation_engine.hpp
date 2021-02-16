#ifndef TERMOX_SYSTEM_ANIMATION_ENGINE_HPP
#define TERMOX_SYSTEM_ANIMATION_ENGINE_HPP
#include <algorithm>
#include <chrono>
#include <future>
#include <iterator>
#include <map>
#include <mutex>

#include <termox/common/lockable.hpp>
#include <termox/system/detail/interval_event_loop.hpp>

namespace ox {
class Widget;

/// Registers Widgets with intervals to send timer events.
class Animation_engine : public detail::Interval_event_loop,
                         private Lockable<std::recursive_mutex> {
    class Animation_event_loop : detail::Interval_event_loop {};

   private:
    using Clock_t    = std::chrono::steady_clock;
    using Time_point = Clock_t::time_point;

    struct Registered_data {
        std::chrono::milliseconds interval;
        Time_point last_event_time;
    };

   public:
    using Interval_t = std::chrono::milliseconds;

   public:
    Animation_engine() : Interval_event_loop{Interval_t{60}} {}

    ~Animation_engine()
    {
        this->Interval_event_loop::exit(0);
        this->Interval_event_loop::wait();
    }

   public:
    /// Register to start sending Timer_events to \p w every \p interval.
    void register_widget(Widget& w, Interval_t interval);

    /// Register to start sending Timer_events to \p w at \p fps.
    void register_widget(Widget& w, FPS fps);

    /// Stop the given Widget from being sent Timer_events.
    void unregister_widget(Widget& w);

   private:
    std::map<Widget*, Registered_data> subjects_;

   private:
    void loop_function();

    /// Post any timer events that are ready to be posted.
    void post_timer_events();

    /// Only update if \p interval half is less than the current set interval.
    void maybe_update_interval(Interval_t interval)
    {
        // mutex is already locked
        auto const current_interval = this->Interval_event_loop::get_interval();
        if (auto const half = interval / 2; half < current_interval)
            this->set_interval(half);
    }

    /// Finds the smallest registered interval, uses it for class interval.
    void reset_interval()
    {
        // mutex is already locked
        auto const iter =
            std::min_element(std::cbegin(subjects_), std::cend(subjects_),
                             [](auto const& a, auto const& b) {
                                 return a.second.interval < b.second.interval;
                             });
        if (iter == std::cend(subjects_))
            return;
        this->Interval_event_loop::set_interval(iter->second.interval / 2);
    }
};

}  // namespace ox
#endif  // TERMOX_SYSTEM_ANIMATION_ENGINE_HPP

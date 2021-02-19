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
   public:
    using Interval_event_loop::Clock_t;
    using Interval_event_loop::Interval_t;
    using Interval_event_loop::Time_point;

    struct Registered_data {
        Interval_t interval;
        Time_point last_event_time;
    };

    static auto constexpr default_interval = Interval_t{100};

   public:
    Animation_engine() : Interval_event_loop{default_interval} {}

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

    /// Return true if there are no registered widgets
    [[nodiscard]] auto is_empty() const -> bool { return subjects_.empty(); }

   private:
    std::map<Widget*, Registered_data> subjects_;

   private:
    void loop_function() override;

    /// Post any timer events that are ready to be posted.
    /** Returns true if any events were posted. */
    void post_timer_events();
};

}  // namespace ox
#endif  // TERMOX_SYSTEM_ANIMATION_ENGINE_HPP

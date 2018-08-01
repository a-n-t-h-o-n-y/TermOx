#ifndef CPPURSES_SYSTEM_DETAIL_ANIMATION_EVENT_LOOP_HPP
#define CPPURSES_SYSTEM_DETAIL_ANIMATION_EVENT_LOOP_HPP
#include <chrono>
#include <functional>
#include <future>
#include <unordered_map>

#include <signals/connection.hpp>
#include <signals/signal.hpp>

#include <cppurses/system/event_loop.hpp>

namespace cppurses {
class Widget;
namespace detail {

class Animation_event_loop : public Event_loop {
   public:
    using Period_t = std::chrono::milliseconds;
    /// Create an Animation Loop with a variable period.
    Animation_event_loop(std::function<Period_t()> period_func);

    Animation_event_loop(Animation_event_loop&&) = default;
    Animation_event_loop& operator=(Animation_event_loop&&) = default;

    /// Sends exit signal and waits for return from async thread.
    ~Animation_event_loop();

    /// Create an Animation Loop with a constant period.
    Animation_event_loop(Period_t period);

    /// Launches the Event_loop::run() in a separate thread.
    void run();

    /// Blocks until the async event loop returns.
    ///
    /// Undefined behavior if you call before you have called run().
    /// Event_loop::exit(int) must be called to return from wait().
    /// @return the return code passed to the call to exit().
    int wait();

    /// Register a widget to have an Animation_event posted to it every period.
    ///
    /// No-op if widget is already registered.
    void register_widget(Widget& w);

    /// Stop a widget from recieving animation events for this loop.
    bool unregister_widget(Widget& w);

    /// Sets a new constant period for the animation loop.
    void set_period(Period_t period);

    /// Sets a new variable period for the animation loop.
    void set_period(const std::function<Period_t()>& period_function);

    /// Returns true if no Widgets are registered with this event loop.
    bool empty() const;

   private:
    void post_all();

    sig::Signal<void()> on_loop_;
    std::unordered_map<Widget*, sig::Connection> connections_;
    std::function<Period_t()> period_func_;
    std::future<int> fut_;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_time_;
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_ANIMATION_EVENT_LOOP_HPP

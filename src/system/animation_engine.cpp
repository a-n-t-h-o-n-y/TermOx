#include <termox/system/animation_engine.hpp>

#include <algorithm>
#include <iterator>
#include <memory>

#include <termox/system/detail/timer_event_loop.hpp>

namespace ox {

void Animation_engine::register_widget(Widget& w, Period_t interval)
{
    if (!this->has_loop_with(interval)) {
        loops_.emplace_back(
            std::make_unique<detail::Timer_event_loop>(interval));
        loops_.back()->run_async();
    }
    this->get_loop_with(interval).register_widget(w);
}

void Animation_engine::register_widget(Widget& w, FPS fps)
{
    this->register_widget(w, detail::Interval_event_loop::fps_to_period(fps));
}

void Animation_engine::unregister_widget(Widget& w)
{
    // Unregister the Widget from the event loop it is contained in, and return
    // that event loop.
    auto const iter = std::find_if(
        std::begin(loops_), std::end(loops_),
        [&w](auto const& loop) { return loop->unregister_widget(w); });

    // If an event loop was found, and it is now empty, shutdown that event loop
    if (iter != std::end(loops_)) {
        if (auto& loop = *iter; loop->is_empty()) {
            loop->exit(0);
            loop->wait();
            loops_.erase(iter);
        }
    }
}

void Animation_engine::shutdown()
{
    /* Timer_event_loops will wait on the future at destruction.
     * Because shutdown is called from Event_loop and will wait forever. */
    for (auto& loop : loops_) {
        loop->exit(0);
        loop->wait();
    }
}

void Animation_engine::startup()
{
    for (auto& loop : loops_)
        loop->run_async();
}

}  // namespace ox

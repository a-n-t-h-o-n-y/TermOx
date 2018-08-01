#include <cppurses/system/animation_engine.hpp>

#include <algorithm>
#include <chrono>
#include <functional>

#include <signals/slot.hpp>

#include <cppurses/painter/paint_buffer.hpp>
#include <cppurses/system/events/animation_event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

void Animation_engine::register_widget(Widget& w, Period_t period) {
    if (const_loops_.count(period) == 0) {
        auto pair = const_loops_.emplace(period, period);
        pair.first->second.run();
    }
    auto iter = const_loops_.find(period);
    if (iter != std::end(const_loops_)) {
        iter->second.register_widget(w);
    }
}

void Animation_engine::register_widget(
    Widget& w,
    const std::function<Period_t()>& period_func) {
    variable_loops_.emplace_back(period_func);
    variable_loops_.back().register_widget(w);
    variable_loops_.back().run();
}

void Animation_engine::unregister_widget(Widget& w) {
    for (auto iter = std::begin(const_loops_); iter != std::end(const_loops_);
         ++iter) {
        if (iter->second.unregister_widget(w)) {
            if (iter->second.empty()) {
                const_loops_.erase(iter);
                return;
            }
        }
    }
    for (auto iter = std::begin(variable_loops_);
         iter != std::end(variable_loops_); ++iter) {
        if (iter->unregister_widget(w)) {
            if (iter->empty()) {
                variable_loops_.erase(iter);
                return;
            }
        }
    }
}

void Animation_engine::shutdown() {
    // Animation_event_loops will wait on the future at destruction.
    // Because shutdown is called from Event_loop and will wait forever.
    for (auto& pair : const_loops_) {
        pair.second.exit(0);
    }
    for (auto& loop : variable_loops_) {
        loop.exit(0);
    }
}

void Animation_engine::startup() {
    for (auto& pair : const_loops_) {
        pair.second.run();
    }
    for (auto& loop : variable_loops_) {
        loop.run();
    }
}

Event_loop* Animation_engine::get_event_loop(std::thread::id id) {
    for (auto& pair : const_loops_) {
        if (pair.second.get_thread_id() == id) {
            return &pair.second;
        }
    }
    for (auto& loop : variable_loops_) {
        if (loop.get_thread_id() == id) {
            return &loop;
        }
    }
    return nullptr;
}

}  // namespace cppurses

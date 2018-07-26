#include <cppurses/system/animation_engine.hpp>

#include <algorithm>
#include <chrono>
#include <functional>

#include <signals/slot.hpp>

#include <cppurses/painter/paint_buffer.hpp>
#include <cppurses/system/async_trigger.hpp>
#include <cppurses/system/events/animation_event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
Animation_event_loop::Period_t fps_to_period(int fps) {
    // TODO fix logic, should use the compile time ratio of Period_t??
    return Animation_event_loop::Period_t(
        static_cast<int>((1.0 / static_cast<double>(fps)) * 1'000));
}

// namespace slot {

// sig::Slot<void()> animation_event(Widget* widg) {
//     sig::Slot<void()> slot{
//         [widg] { System::send_event(Animation_event{widg}); }};
//     slot.track(widg->destroyed);
//     return slot;
// }

// sig::Slot<void()> flush() {
//     sig::Slot<void()> slot{[] { System::paint_buffer().flush(true); }};
//     return slot;
// }

// }  // namespace slot

void Animation_engine::register_widget(Widget& w, Period_t period) {
    if (const_loops_.count(period) == 0) {
        const_loops_.emplace(period, period);
        const_loops_[period].run();
    }
    const_loops_[period].register_widget(w);
}

// void Animation_engine::register_widget(Widget* widg, int frames_per_second) {
//     // Check first that the widget does not have an active connection.
//     if (connections_.count(widg) == 1) {
//         if (connections_.at(widg).connected()) {
//             return;
//         }
//         // Add to existing framerate Async trigger.
//     } else if (const_framerate_triggers_.count(frames_per_second) == 1) {
//         auto conn = const_framerate_triggers_.at(frames_per_second)
//                         .trigger.connect(slot::animation_event(widg),
//                                          sig::Position::at_front);
//         connections_[widg] = conn;
//     } else {  // Add new Async_trigger to map.
//         const_framerate_triggers_[frames_per_second] =
//             Async_trigger{fps_to_period(frames_per_second)};
//         auto conn = const_framerate_triggers_.at(frames_per_second)
//                         .trigger.connect(slot::animation_event(widg),
//                                          sig::Position::at_front);
//         const_framerate_triggers_.at(frames_per_second)
//             .trigger.connect(slot::flush(), sig::Position::at_back);
//         const_framerate_triggers_[frames_per_second].start();
//         connections_[widg] = conn;
//     }
// }

void Animation_engine::register_widget(Widget& w,
                                       std::function<Period_t()> period_func) {
    variable_loops_.emplace_back(period_func);
    variable_loops_.back().register_widget(w);
    variable_loops_.back().run();
}

// void Animation_engine::register_widget(Widget* widg,
//                                        std::function<int()>
//                                        frames_per_second) {
//     // Check first that the widget does not have an active connection.
//     if (connections_.count(widg) == 1) {
//         if (connections_.at(widg).connected()) {
//             return;
//         }
//     } else {  // Add new Async_trigger to vector.
//         variable_framerate_triggers_.push_back(
//             Async_trigger{[frames_per_second] {
//                 return fps_to_period(frames_per_second());
//             }});
//         auto conn = variable_framerate_triggers_.back().trigger.connect(
//             slot::animation_event(widg));
//         variable_framerate_triggers_.back().trigger.connect(
//             slot::flush(), sig::Position::at_back);
//         variable_framerate_triggers_.back().start();
//         connections_[widg] = conn;
//     }
// }

void Animation_engine::unregister_widget(Widget& w) {
    for (auto& pair : const_loops_) {
        pair.second.unregister(w);
    }
    for (auto& loop : variable_loops_) {
        loop.unregister(w);
    }
    this->clean_up();
}

void Animation_engine::clean_up() {
    auto const_end_iter =
        std::remove_if(std::begin(const_loops_), std::end(const_loops),
                       [](const auto& pair) { return pair.second.empty(); });
    const_loops_.erase(const_end_iter, std::end(const_loops_));
    auto var_end_iter =
        std::remove_if(std::begin(variable_loops_), std::end(variable_loops),
                       [](const auto& loop) { return loop.empty(); });
    variable_loops_.erase(var_end_iter, std::end(variable_loops_));
}

// void Animation_engine::deregister_widget(Widget* widg) {
//     if (connections_.count(widg) == 1) {
//         connections_.at(widg).disconnect();
//     }
//     // eventually stop the async_trigger object if it is a function or if
//     there
//     // are no more at that framerate.
// }

void Animation_engine::shutdown() {
    for (auto& pair : const_loops_) {
        pair.second.exit(0);
        pair.second.wait();
    }
    for (auto& loop : variable_loops_) {
        loop.exit(0);
        loop.wait();
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

// void Animation_engine::shutdown() {
//     // const rates
//     for (auto& rate_trigger_pair : const_framerate_triggers_) {
//         rate_trigger_pair.second.stop();
//         rate_trigger_pair.second.wait();
//     }
//     // variable rates
//     for (auto& trigger : variable_framerate_triggers_) {
//         trigger.stop();
//         trigger.wait();
//     }
// }

}  // namespace cppurses

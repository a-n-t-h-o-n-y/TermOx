#include <cppurses/system/animation_engine.hpp>

#include <chrono>
#include <functional>

#include <signals/slot.hpp>

#include <cppurses/system/async_trigger.hpp>
#include <cppurses/system/events/animation_event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/widget.hpp>

namespace {

std::chrono::nanoseconds fps_to_period(int fps) {
    return std::chrono::milliseconds((int)((1.0 / (double)fps) * 1'000));
}

}  // namespace

namespace cppurses {
namespace slot {

sig::Slot<void()> animation_event(Widget* widg) {
    // sig::Slot<void()> slot{[widg] { widg->animation_event(); }};
    sig::Slot<void()> slot{
        [widg] { System::send_event(Animation_event{widg}); }};
    slot.track(widg->destroyed);
    return slot;
}

}  // namespace slot

void Animation_engine::register_widget(Widget* widg, int frames_per_second) {
    // Check first that the widget does not have an active connection.
    if (connections_.count(widg) == 1) {
        if (connections_.at(widg).connected()) {
            return;
        }
    } else if (const_framerate_triggers_.count(frames_per_second) == 1) {
        auto conn = const_framerate_triggers_.at(frames_per_second)
                        .trigger.connect(slot::animation_event(widg));
        connections_[widg] = conn;
    } else {
        const_framerate_triggers_[frames_per_second] =
            Async_trigger{fps_to_period(frames_per_second)};
        auto conn = const_framerate_triggers_.at(frames_per_second)
                        .trigger.connect(slot::animation_event(widg));
        const_framerate_triggers_[frames_per_second].start();
        connections_[widg] = conn;
    }
}

void Animation_engine::register_widget(Widget* widg,
                                       std::function<int()> frames_per_second) {
    // Check first that the widget does not have an active connection.
    if (connections_.count(widg) == 1) {
        if (connections_.at(widg).connected()) {
            return;
        }
    } else {
        variable_framerate_triggers_.push_back(
            Async_trigger{[frames_per_second] {
                return fps_to_period(frames_per_second());
            }});
        auto conn = variable_framerate_triggers_.back().trigger.connect(
            slot::animation_event(widg));
        variable_framerate_triggers_.back().start();
        connections_[widg] = conn;
    }
}

void Animation_engine::deregister_widget(Widget* widg) {
    if (connections_.count(widg) == 1) {
        connections_.at(widg).disconnect();
    }
    // eventually stop the async_trigger object if it is a function or if there
    // are no more at that framerate.
}

void Animation_engine::shutdown() {
    // const rates
    for (auto& rate_trigger_pair : const_framerate_triggers_) {
        rate_trigger_pair.second.stop();
        rate_trigger_pair.second.wait();
    }
    // variable rates
    for (auto& trigger : variable_framerate_triggers_) {
        trigger.stop();
        trigger.wait();
    }
}

}  // namespace cppurses

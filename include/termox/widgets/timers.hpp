#pragma once

#include <algorithm>
#include <chrono>
#include <memory>
#include <optional>

#include <termox/core.hpp>
#include <termox/widgets/widget.hpp>

namespace ox::widgets {

// class Timers {
//    public:
//     struct Handle {
//         int id;
//     };

//    public:
//     /**
//      * Register a widget to receive timer events.
//      * @param w The widget to register.
//      * @param period The time between timer events.
//      * @return A handle to the registered timer.
//      */
//     [[nodiscard]] static auto enroll(Widget& w,
//                                      std::chrono::milliseconds period) -> Handle
//     {
//         auto [iter, _] = timers_.emplace(Timer{period, true}, w.track());
//         return {iter->first.id()};
//     }

//     /**
//      * Unregister a widget from receiving timer events.
//      * @param h The handle to the timer to unregister.
//      */
//     static auto unenroll(Handle h) -> void
//     {
//         auto const iter = std::ranges::find_if(
//             timers_, [h](auto const& pair) { return pair.first.id() == h.id; });

//         if (iter != std::end(timers_)) {
//             timers_.erase(iter);
//         }
//     }

//     [[nodiscard]] static auto get_receiver(Handle h) -> std::optional<Widget>
//     {
//         auto const iter = std::ranges::find_if(
//             timers_, [h](auto const& pair) { return pair.first.id() == h.id; });

//         if (iter == std::end(timers_)) {
//             return std::nullopt;
//         }
//         else {
//             return Widget{iter->second};
//         }
//     }

//    private:
//     struct TimerCompare {
//         auto operator()(Timer const& lhs, Timer const& rhs) const -> bool
//         {
//             return lhs.id() < rhs.id();
//         }
//     };

//     using TimerMap =
//         std::map<Timer, std::weak_ptr<Widget::Concept>, TimerCompare>;

//     inline static TimerMap timers_;
// };

}  //  namespace ox::widgets
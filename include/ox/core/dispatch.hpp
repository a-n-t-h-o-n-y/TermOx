/** @file
 * Event delivery and effect propagation without terminal I/O.
 */
#pragma once

#include <concepts>
#include <type_traits>

#include <ox/core/canvas.hpp>
#include <ox/core/events.hpp>

namespace ox {
/** Deliver a typed event when a matching handle overload exists. No effects are
 * applied here; the event loop owns repaint and exit decisions. */
template <typename Handler, typename Input>
[[nodiscard]] auto dispatch(Handler& handler, Input const& input) -> EventResponse
{
    if constexpr (requires {
                      { handler.handle(input) } -> std::same_as<EventResponse>;
                  }) {
        return handler.handle(input);
    }
    else {
        return {};
    }
}

/** Process loop commands and resize storage, then deliver application input. */
template <typename Handler>
[[nodiscard]] auto dispatch(Event const& event, Handler& handler, Frame& screen)
    -> EventResponse
{
    return std::visit(
        [&](auto const& input) -> EventResponse {
            using Input = std::remove_cvref_t<decltype(input)>;
            if constexpr (std::same_as<Input, event::Custom>) { return input.action(); }
            else if constexpr (std::same_as<Input, event::Interrupt>) {
                return {.handled = true, .quit = 1};
            }
            else if constexpr (std::same_as<Input, esc::Resize>) {
                screen.resize(input.size);
                return combine({.repaint = true}, dispatch(handler, input));
            }
            else {
                return dispatch(handler, input);
            }
        },
        event);
}
}  // namespace ox

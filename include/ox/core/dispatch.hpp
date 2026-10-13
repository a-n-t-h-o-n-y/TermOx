#pragma once

#include <zzz/overload.hpp>

#include <ox/core/canvas.hpp>
#include <ox/core/events.hpp>

namespace ox {
/**
 * Calls the appropriate handler function on \p handler for the given Event.
 *
 * @details This will only call the handler if the handler has the appropriate function
 * for the given Event type.
 * @param ev The Event to handle.
 * @param handler The event handler to process the event.
 * @param buffer The Frame to write changes to on paint event.
 * @return Handling, repaint, and exit decisions.
 */
template <typename T>
[[nodiscard]] auto apply_event(Event const& ev, T& handler, Frame& buffer)
    -> EventResponse
{
    return std::visit(
        zzz::Overload{
            [&](esc::KeyEvent const& e) -> EventResponse {
                if (e.action == KeyAction::Release) {
                    if constexpr (HandlesKeyRelease<T>) {
                        return handler.handle_key_release(e);
                    }
                }
                else {
                    if constexpr (HandlesKeyPress<T>) {
                        return handler.handle_key_press(e);
                    }
                }
                return {};
            },
            [&](esc::MousePress e) -> EventResponse {
                if constexpr (HandlesMousePress<T>) {
                    return handler.handle_mouse_press(e);
                }
                else {
                    return {};
                }
            },
            [&](esc::MouseRelease e) -> EventResponse {
                if constexpr (HandlesMouseRelease<T>) {
                    return handler.handle_mouse_release(e);
                }
                else {
                    return {};
                }
            },
            [&](esc::MouseWheel e) -> EventResponse {
                if constexpr (HandlesMouseWheel<T>) {
                    return handler.handle_mouse_wheel(e);
                }
                else {
                    return {};
                }
            },
            [&](esc::MouseMove e) -> EventResponse {
                if constexpr (HandlesMouseMove<T>) {
                    return handler.handle_mouse_move(e);
                }
                else {
                    return {};
                }
            },
            [&](esc::Resize e) -> EventResponse {
                buffer.resize(e.size);
                if constexpr (HandlesResize<T>) {
                    auto result = handler.handle_resize(e.size);
                    result.repaint = true;
                    return result;
                }
                else {
                    return {.repaint = true};
                }
            },
            [&](event::Timer e) -> EventResponse {
                if constexpr (HandlesTimer<T>) { return handler.handle_timer(e.id); }
                else {
                    return {};
                }
            },
            [](event::Custom const& e) -> EventResponse { return e.action(); },
            [](event::Interrupt) -> EventResponse {
                return {.handled = true, .quit = 1};
            },
            [&](esc::PasteChunk const& e) -> EventResponse {
                if constexpr (requires { handler.handle_paste(e); }) {
                    return handler.handle_paste(e);
                }
                return {};
            },
            [](auto const&) -> EventResponse { return {}; }},
        ev);
}

template <typename T>
concept HandlesPaint = requires(T t, Canvas c) {
    { t.handle_paint(c) } -> std::same_as<Cursor>;
};

}  // namespace ox

#pragma once

#include <ox/core.hpp>

namespace ox {

/**
 * Top level application class that handles event processing and painting.
 */
template <typename HeadWidget>
class Application {
   public:
    /**
     * Construct with a widget to be the head of the application and initialize the
     * terminal screen.
     */
    explicit Application(HeadWidget head, Terminal term = {})
        : head_{std::move(head)}, term_{std::move(term)}
    {}

   public:
    /**
     * Run the application, processing events and painting the screen. Blocking.
     */
    auto run() -> int { return process_events(term_, *this); }

   public:
    auto handle_mouse_press(Mouse m) -> EventResponse
    {
        if constexpr (requires(HeadWidget& w, Mouse m) { mouse_press(w, m); }) {
            mouse_press(head_, m);
        }
        return {};
    }

    auto handle_mouse_release(Mouse m) -> EventResponse
    {
        if constexpr (requires(HeadWidget& w, Mouse m) { mouse_release(w, m); }) {
            mouse_release(head_, m);
        }
        return {};
    }

    auto handle_mouse_wheel(Mouse m) -> EventResponse
    {
        if constexpr (requires(HeadWidget& w, Mouse m) { mouse_wheel(w, m); }) {
            mouse_wheel(head_, m);
        }
        return {};
    }

    auto handle_mouse_move(Mouse m) -> EventResponse
    {
        if constexpr (requires(HeadWidget& w, Mouse m) { mouse_move(w, m); }) {
            mouse_move(head_, m);
        }
        return {};
    }

    auto handle_key_press(Key k) -> EventResponse
    {
        if constexpr (requires(HeadWidget& w, Key k) { key_press(w, k); }) {
            // TODO send to focus widget instead? Or layout implements that logic?
            key_press(head_, k);
        }
        return {};
    }

    auto handle_key_release(Key k) -> EventResponse
    {
        if constexpr (requires(HeadWidget& w, Key k) { key_release(w, k); }) {
            // TODO send to focus widget instead? Or layout implements that logic?
            key_release(head_, k);
        }
        return {};
    }

    auto handle_resize(Area new_size) -> EventResponse
    {
        if constexpr (requires(HeadWidget& w, Area a) { resize(w, a); }) {
            resize(head_, new_size);
        }
        return {};
    }

    auto handle_timer(int id) -> EventResponse
    {
        if constexpr (requires(HeadWidget& w, int i) { timer(w, i); }) {
            timer(head_, id);
        }
        return {};
    }

    auto handle_paint(Canvas canvas) const -> Terminal::Cursor
    {
        if constexpr (requires(HeadWidget const& w, Canvas c) { paint(w, c); }) {
            paint(head_, canvas);
        }
        // TODO cursor return from focus widget
        // find focus widget
        // ask it for cursor or this is one function that is called on head and the
        // cursor function will search for focus widget itself and return its cursor if
        // found.
        return {};
    }

   private:
    HeadWidget head_;
    Terminal term_;
    // Point previous_mouse\position_{0, 0}; // TODO put in layouts?
};

}  // namespace ox
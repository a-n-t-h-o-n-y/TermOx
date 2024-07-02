#pragma once

#include <cstdint>

#include <ox/core.hpp>
#include <ox/widgets/widget.hpp>

namespace ox {

/**
 * Top level Application class that handles event processing and painting.
 */
class Application {
   public:
    /**
     * Create an Application that will forward events to the given head Widget and use
     * the given Terminal to paint the screen.
     */
    explicit Application(Widget& head, Terminal term = {});

   public:
    /**
     * Run the application, processing events and painting the screen. Blocking.
     */
    auto run() -> int;

   public:
    auto handle_mouse_press(Mouse m) -> EventResponse;

    auto handle_mouse_release(Mouse m) -> EventResponse;

    auto handle_mouse_wheel(Mouse m) -> EventResponse;

    auto handle_mouse_move(Mouse m) -> EventResponse;

    auto handle_key_press(Key k) -> EventResponse;

    auto handle_key_release(Key k) -> EventResponse;

    auto handle_resize(Area new_size) -> EventResponse;

    auto handle_timer(int id) -> EventResponse;

    auto handle_paint(Canvas canvas) const -> Terminal::Cursor;

   private:
    Widget& head_;
    Terminal term_;
    Point previous_mouse_position_{0, 0};
};

}  // namespace ox
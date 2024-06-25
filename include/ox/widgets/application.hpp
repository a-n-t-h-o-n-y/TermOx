#pragma once

#include <cstdint>

#include <ox/core.hpp>
#include <ox/widgets/widget.hpp>

namespace ox {

/**
 * Top level application class that handles event processing and painting.
 */
class Application {
   public:
    /**
     * Construct with a widget to be the head of the application and initialize the
     * terminal screen.
     */
    template <typename T>
    explicit Application(T head, Terminal term = {})
        : head_{std::move(head)}, term_{std::move(term)}
    {}
    // TODO ^^ possibly make head hardcoded a layout so focus is handled, then this
    // 'head' is a child of the actual head layout.

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
    Widget head_;
    Terminal term_;
    Point previous_mouse_position_{0, 0};
};

}  // namespace ox
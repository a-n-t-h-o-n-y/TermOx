#pragma once

#include <cstdint>

#include <termox/core.hpp>
#include <termox/widgets/widget.hpp>

namespace ox::widgets {

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
    explicit Application(T head, ox::Terminal term = {})
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
    auto handle_mouse_press(ox::Mouse m) -> ox::EventResponse;

    auto handle_mouse_release(ox::Mouse m) -> ox::EventResponse;

    auto handle_mouse_wheel(ox::Mouse m) -> ox::EventResponse;

    auto handle_mouse_move(ox::Mouse m) -> ox::EventResponse;

    auto handle_key_press(ox::Key k) -> ox::EventResponse;

    auto handle_key_release(ox::Key k) -> ox::EventResponse;

    auto handle_resize(ox::Area new_size) -> ox::EventResponse;

    auto handle_timer(int id) -> ox::EventResponse;

    auto handle_paint(ox::Canvas canvas) const -> ox::Terminal::Cursor;

   private:
    Widget head_;
    ox::Terminal term_;
    Point previous_mouse_position_{0, 0};
};

}  // namespace ox::widgets
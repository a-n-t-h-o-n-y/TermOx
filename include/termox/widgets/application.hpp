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
     * Construct with a widget to be the head of the application and initialize
     * the terminal screen.
     */
    template <typename T>
    explicit Application(T head)
        : head_{std::move(head), {.focus_policy = FocusPolicy::None}}
    {}
    // TODO ^^ possibly make head hardcoded a layout so focus is handled, then
    // this 'head' is a child of the actual head layout.

   public:
    /**
     * Run the application, processing events and painting the screen. Blocking.
     */
    auto run() -> int;

   public:
    auto handle_mouse_press(ox::Mouse m) -> ox::EventResponse
    {
        mouse_press(head_, m);
        paint(head_, ox::Canvas{.at = {0, 0}, .size = term_.changes.size()});
        return {};
    }

    auto handle_mouse_release(ox::Mouse m) -> ox::EventResponse
    {
        mouse_release(head_, m);
        paint(head_, ox::Canvas{.at = {0, 0}, .size = term_.changes.size()});
        return {};
    }

    auto handle_mouse_wheel(ox::Mouse m) -> ox::EventResponse
    {
        mouse_wheel(head_, m);
        paint(head_, ox::Canvas{.at = {0, 0}, .size = term_.changes.size()});
        return {};
    }

    auto handle_mouse_move(ox::Mouse m) -> ox::EventResponse
    {
        mouse_move(head_, m);
        paint(head_, ox::Canvas{.at = {0, 0}, .size = term_.changes.size()});
        return {};
    }

    auto handle_key_press(ox::Key k) -> ox::EventResponse;

    auto handle_key_release(ox::Key k) -> ox::EventResponse
    {
        auto focused = Focus::get();
        if (focused != nullptr) {
            key_release(*focused, k);
        }
        paint(head_, ox::Canvas{.at = {0, 0}, .size = term_.changes.size()});
        return {};
    }

    auto handle_resize(ox::Area new_size) -> ox::EventResponse
    {
        resize(head_, new_size);
        paint(head_, ox::Canvas{.at = {0, 0}, .size = new_size});
        return {};
    }

    auto handle_timer(int id) -> ox::EventResponse
    {
        timer(head_, id);
        paint(head_, ox::Canvas{.at = {0, 0}, .size = term_.changes.size()});
        return {};
    }

   private:
    Widget head_;
    ox::Terminal term_;
};

}  // namespace ox::widgets
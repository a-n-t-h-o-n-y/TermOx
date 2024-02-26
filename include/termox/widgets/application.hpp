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
    explicit Application(T head) : head_{std::move(head)}
    {}

   public:
    auto run() -> int
    {
        ox::Terminal::event_queue.append(esc::Resize{term_.area()});
        return process_events(term_, *this);
    }

   public:
    auto handle_mouse_press(ox::Mouse m) -> ox::EventResponse
    {
        mouse_press(head_, m);
        paint(head_, ox::Canvas{.at = {0, 0}, .size = term_.changes.area()});
        return {};
    }

    auto handle_mouse_release(ox::Mouse m) -> ox::EventResponse
    {
        mouse_release(head_, m);
        paint(head_, ox::Canvas{.at = {0, 0}, .size = term_.changes.area()});
        return {};
    }

    auto handle_mouse_wheel(ox::Mouse m) -> ox::EventResponse
    {
        mouse_wheel(head_, m);
        paint(head_, ox::Canvas{.at = {0, 0}, .size = term_.changes.area()});
        return {};
    }

    auto handle_mouse_move(ox::Mouse m) -> ox::EventResponse
    {
        mouse_move(head_, m);
        paint(head_, ox::Canvas{.at = {0, 0}, .size = term_.changes.area()});
        return {};
    }

    auto handle_key_press(ox::Key k) -> ox::EventResponse
    {
        // TODO a shortcuts manager? processed here?
        auto focused = Focus::get();
        if (focused != nullptr) {
            key_press(*focused, k);
        }
        paint(head_, ox::Canvas{.at = {0, 0}, .size = term_.changes.area()});
        return {};
    }

    auto handle_key_release(ox::Key k) -> ox::EventResponse
    {
        auto focused = Focus::get();
        if (focused != nullptr) {
            key_release(*focused, k);
        }
        paint(head_, ox::Canvas{.at = {0, 0}, .size = term_.changes.area()});
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
        // auto w = Timers::get_receiver(Timers::Handle{id});
        // if (w.has_value()) {
        //     timer(*w);
        // }
        paint(head_, ox::Canvas{.at = {0, 0}, .size = term_.changes.area()});
        return {};
    }

   private:
    Widget head_;
    ox::Terminal term_;
};

}  // namespace ox::widgets
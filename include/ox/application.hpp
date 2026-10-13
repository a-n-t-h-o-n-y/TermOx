#pragma once

#include <ox/core/core.hpp>
#include <ox/focus.hpp>
#include <ox/widget.hpp>

namespace ox {

/** Owns widget interaction and a terminal-independent event loop. */
class Application {
   public:
    explicit Application(Widget& head);
    ~Application();

    Application(Application const&) = delete;
    auto operator=(Application const&) -> Application& = delete;

    /** Open a terminal and drive the application on the calling thread. */
    auto run(Terminal::Options options = {}) -> int;

    /** Dispatch a batch and due timers without terminal I/O. */
    auto step(EventLoop::Clock::time_point now = EventLoop::Clock::now())
        -> EventResponse;
    auto paint() -> bool;
    void post(Event event);
    void request_repaint();
    void quit(int code = 0);

    auto loop() -> EventLoop& { return loop_; }
    auto focus() -> Focus& { return focus_; }

    auto handle_mouse_press(MousePress m) -> EventResponse;
    auto handle_mouse_release(MouseRelease m) -> EventResponse;
    auto handle_mouse_wheel(MouseWheel m) -> EventResponse;
    auto handle_mouse_move(MouseMove m) -> EventResponse;
    auto handle_key_press(KeyEvent k) -> EventResponse;
    auto handle_key_release(KeyEvent k) -> EventResponse;
    auto handle_paste(esc::PasteChunk const& chunk) -> EventResponse;
    auto handle_resize(Area new_size) -> EventResponse;
    auto handle_paint(Canvas canvas) -> Cursor;

   private:
    Widget& head_;
    EventLoop loop_;
    Focus focus_;
    std::vector<LifetimeView<Widget>> attached_;
    Point previous_mouse_position_{0, 0};

    void attach_widgets();
};
}  // namespace ox

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

    /** Whether a widget belongs to the active, enabled input tree. */
    auto is_interactive(Widget const& widget) const -> bool;

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
    std::vector<WidgetNode> tree_;
    std::optional<Point> mouse_position_;
    std::vector<LifetimeView<Widget>> hovered_;

    auto synchronize() -> EventResponse;
    auto update_hover() -> EventResponse;
    void report(EventResponse response);
    template <typename Input>
    auto route_mouse(Input event,
                     EventResponse (Widget::*callback)(Input),
                     bool select = false) -> EventResponse;
    template <typename Callback>
    auto route_focused(Callback callback) -> EventResponse;

    void attach_widgets();
};
}  // namespace ox

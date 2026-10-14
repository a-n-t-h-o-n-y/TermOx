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
    auto focus() const -> Focus const& { return focus_; }
    void set_focus(Widget& widget);
    void clear_focus();
    /** Immediate delivery and effect application; post() defers delivery. */
    auto dispatch(Event const& event) -> EventResponse;

    /** Whether a widget belongs to the visible, enabled input tree. */
    auto is_interactive(Widget const& widget) const -> bool;

    auto handle(MousePress const& m) -> EventResponse;
    auto handle(MouseRelease const& m) -> EventResponse;
    auto handle(MouseWheel const& m) -> EventResponse;
    auto handle(MouseMove const& m) -> EventResponse;
    auto handle(KeyEvent const& k) -> EventResponse;
    auto handle(esc::PasteChunk const& chunk) -> EventResponse;
    auto handle(esc::Resize const& event) -> EventResponse;
    auto paint(Canvas canvas) -> Cursor;

   private:
    Widget& head_;
    EventLoop loop_;
    Focus focus_;
    std::vector<LifetimeView<Widget>> attached_;
    std::vector<WidgetNode> tree_;
    std::vector<WidgetNode> tree_storage_;
    std::vector<LifetimeView<Widget>> attachment_storage_;
    bool attachments_current_ = false;
    std::optional<Point> mouse_position_;
    std::vector<LifetimeView<Widget>> hovered_;
    LifetimeView<Widget> paste_target_;

    auto synchronize() -> EventResponse;
    auto update_hover() -> std::pair<EventResponse, bool>;
    auto refresh_tree() -> bool;
    template <typename Input>
    auto route_mouse(Input event, bool select = false) -> EventResponse;
    template <typename Input>
    auto route_focused(Input const& event) -> EventResponse;

    void attach_widgets();
};
}  // namespace ox

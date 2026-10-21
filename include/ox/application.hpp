/** @file
 * Application ownership, routing, focus, and terminal driving.
 */
#pragma once

#include <ox/clipboard.hpp>
#include <ox/core/core.hpp>
#include <ox/focus.hpp>
#include <ox/widget.hpp>

namespace ox {

/** Owns widget interaction and a terminal-independent event loop.
 * The root is borrowed; construction and headless dispatch do not open a terminal.
 * Except for post(), methods and callbacks run on the application thread. Raw
 * handle() methods return effects; dispatch() delivers and applies those effects.
 * Keep the root alive for the entire run:
 * @code{.cpp}
 * #include <ox/ox.hpp>
 *
 * int main()
 * {
 *     auto quit = ox::Button{{.label = {.text = "Quit"}}};
 *     auto app = ox::Application{quit};
 *     app.set_theme(ox::Theme::dark());
 *     ox::Connection{.signal = quit.on_activate,
 *                    .slot = [&app] { app.quit(); }}();
 *     return app.run();
 * }
 * @endcode
 * @see EventLoop, Widget, Timer
 */
class Application {
   public:
    /** Clipboard callbacks used by attached editors; defaults to application-owned memory. */
    Clipboard clipboard;
    /** Borrow head and attach its widget tree; do not open a terminal.
     * The root must outlive this application. A widget cannot belong to two applications.
     * Initial focus goes to the first eligible Tab/Strong widget.
     * @throws std::logic_error A widget already belongs to another application.
     */
    explicit Application(Widget& head);
    /** Detach surviving widgets, stop their scheduled work, and clear focus. */
    ~Application();

    /** Applications cannot share ownership of their event loop or attachments. */
    Application(Application const&) = delete;
    /** Applications cannot be copy-assigned. */
    auto operator=(Application const&) -> Application& = delete;

    /** Open a terminal and drive the application on the calling thread. */
    auto run(Terminal::Options options = {}) -> int;

    /** Dispatch a batch and due timers without terminal I/O. */
    auto step(EventLoop::Clock::time_point now = EventLoop::Clock::now())
        -> EventResponse;
    /** Paint a pending frame into loop().screen without terminal I/O; return whether a
     * frame was painted.
     */
    auto paint() -> bool;
    /** Thread-safe enqueue and wakeup; delivery occurs on the next step() on the
     * application thread.
     */
    void post(Event event);
    /** Mark a frame pending; repeated requests coalesce. Call on the application thread. */
    void request_repaint();
    /** Schedule root layout and a repaint on the application thread. */
    void request_layout();
    /** Request loop termination with code; subsequent painting is skipped. */
    void quit(int code = 0);

    /** Borrow the event loop and its headless frame storage; do not outlive this
     * application.
     */
    auto loop() -> EventLoop& { return loop_; }
    /** Inspect application-local focus; use set_focus() or clear_focus() to apply
     * transitions.
     */
    auto focus() const -> Focus const& { return focus_; }
    /** Borrow the current visual policy, initially Theme::plain(). */
    auto theme() const -> Theme const& { return theme_; }
    /** Validate and replace the visual policy, notify attached widgets, and request layout and paint.
     * @throws std::invalid_argument Invalid motion durations, frame rate, or palette colors.
     */
    void set_theme(Theme theme);
    /** Apply a focus transition to an eligible widget and its returned effects; invalid
     * targets are rejected.
     */
    void set_focus(Widget& widget);
    /** Notify the old focus owner and keep focus explicitly empty until a later focus
     * command.
     */
    void clear_focus();
    /** Immediate delivery and effect application; post() defers delivery. */
    auto dispatch(Event const& event) -> EventResponse;

    /** Whether a widget belongs to the visible, enabled input tree. */
    auto is_interactive(Widget const& widget) const -> bool;
    /** Return global bounds from a fresh tree snapshot.
     * @throws std::invalid_argument The widget is outside this application.
     * @throws std::logic_error The application root no longer exists. */
    auto bounds(Widget const&) const -> Rect;
    /** Raise an attached overlay above earlier overlays and request painting.
     * @throws std::invalid_argument The widget is not an overlay attached to this application. */
    void raise_overlay(Widget&);

    /** Capture motion and matching release; cancel when owner leaves the input tree. */
    auto capture_pointer(Widget& widget, MouseButton button = MouseButton::Left)
        -> bool;
    /** Release capture only if widget owns it; other widgets cannot release the current
     * owner.
     */
    void release_pointer(Widget& widget);
    /** Borrow the current capture owner; an empty view means no active capture. */
    auto pointer_capture() const -> LifetimeView<Widget> { return pointer_capture_; }

    /** Route a press through the hit ancestry in local coordinates, selecting click
     * focus before delivery.
     */
    auto handle(MousePress const& m) -> EventResponse;
    /** Route a release to the matching capture owner, or through the current hit ancestry. */
    auto handle(MouseRelease const& m) -> EventResponse;
    /** Route wheel input through the current hit ancestry until handled or quitting. */
    auto handle(MouseWheel const& m) -> EventResponse;
    /** Update hover ancestry and route motion, honoring explicit pointer capture. */
    auto handle(MouseMove const& m) -> EventResponse;
    /** Dispatch eligible shortcuts and keyboard input; plain Tab navigates focus. */
    auto handle(KeyEvent const& k) -> EventResponse;
    /** Keep a paste stream with the widget that handled its first chunk, unless that
     * target becomes ineligible.
     */
    auto handle(esc::PasteChunk const& chunk) -> EventResponse;
    /** Resize the root and request painting; used by the event loop driver. */
    auto handle(esc::Resize const& event) -> EventResponse;
    /** Paint visible widgets and overlays into canvas; return the focused widget's
     * clipped global caret.
     */
    auto paint(Canvas canvas) -> Cursor;

   private:
    friend class EventLoop;
    auto paint_widgets(Canvas canvas) -> Cursor;
    std::string clipboard_text_;
    LifetimeView<Widget> head_;
    EventLoop loop_;
    Theme theme_ = Theme::plain();
    Focus focus_;
    std::vector<LifetimeView<Widget>> attached_;
    std::vector<WidgetNode> tree_;
    std::vector<WidgetNode> tree_storage_;
    std::vector<std::pair<std::size_t, LifetimeView<Widget>>> shortcuts_;
    std::vector<LifetimeView<Widget>> attachment_storage_;
    bool attachments_current_ = false;
    bool layout_pending_ = false;
    std::size_t overlay_order_ = 0;
    std::optional<Point> mouse_position_;
    std::vector<LifetimeView<Widget>> hovered_;
    LifetimeView<Widget> paste_target_;
    LifetimeView<Widget> pointer_capture_;
    MouseButton capture_button_ = MouseButton::Left;
    std::size_t capture_revision_ = 0;

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

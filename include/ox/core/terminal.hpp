#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <functional>
#include <map>
#include <optional>
#include <stop_token>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

#include <zzz/overload.hpp>

#include <esc/area.hpp>
#include <esc/event.hpp>
#include <esc/key.hpp>
#include <esc/mouse.hpp>
#include <esc/point.hpp>
#include <esc/terminal.hpp>

#include <ox/core/common.hpp>
#include <ox/core/events.hpp>
#include <ox/core/glyph.hpp>

namespace ox {

using ::esc::Area;
using ::esc::KeyMode;
using ::esc::MouseMode;
using ::esc::Point;
using ::esc::Signals;

/**
 * A 2D Matrix of Glyphs that represents a paintable screen.
 */
class ScreenBuffer {
   public:
    /**
     * Construct a ScreenBuffer with the given dimensions.
     *
     * @param size The dimensions of the ScreenBuffer.
     */
    explicit ScreenBuffer(Area size);

   public:
    /**
     * Access the Glyph at the given position.
     *
     * @details The top left is `{0, 0}` and the bottom right is `{width - 1, height -
     * 1}`. Does no bounds checking.
     * @param p The Point position of the Glyph.
     * @return Glyph& A reference to the Glyph at the given position.
     */
    [[nodiscard]] auto operator[](Point p) -> Glyph&;

    /**
     * Access the Glyph at the given position.
     *
     * @details The top left is `{0, 0}` and the bottom right is `{width - 1, height -
     * 1}`. Does no bounds checking.
     * @param p The Point position of the Glyph.
     * @return Glyph const& A reference to the Glyph at the given position.
     */
    [[nodiscard]] auto operator[](Point p) const -> Glyph const&;

    /**
     * Resize the ScreenBuffer to the given dimensions.
     *
     * @details This does not preserve the contents of the ScreenBuffer, the buffer is
     * left in an undefined state.
     * @param a The new dimensions of the ScreenBuffer.
     */
    void resize(Area a);

    /**
     * Fill the ScreenBuffer with the given Glyph.
     *
     * @param g The Glyph to fill the ScreenBuffer with.
     */
    void fill(Glyph const& g);

    /**
     * Return the size of the ScreenBuffer.
     *
     * @return Area The dimensions of the ScreenBuffer.
     */
    [[nodiscard]] auto size() const -> Area { return size_; }

   private:
    Area size_;
    std::vector<Glyph> buffer_;
};

/**
 * A thread of execution that waits for a given duration and then calls a callback
 * function.
 */
class TimerThread {
   public:
    using CallbackType = std::function<void()>;
    using ClockType = std::chrono::steady_clock;

   public:
    /**
     * Create a placeholder TimerThread that does nothing.
     *
     * @details You'll want to move assign over this to launch a TimerThread.
     */
    TimerThread() = default;

    /**
     * Create a TimerThread that will call the callback after the duration in a loop.
     *
     * @details This will launch a thread immediately.
     * @param duration The periodic duration to wait before calling the callback
     * @param callback The function to call each time the duration has elapsed
     */
    explicit TimerThread(std::chrono::milliseconds duration, CallbackType callback);

    TimerThread(TimerThread const&) = delete;
    TimerThread(TimerThread&&) = default;

    auto operator=(TimerThread const&) -> TimerThread& = delete;
    auto operator=(TimerThread&& x) -> TimerThread& = default;

   public:
    /**
     * Request the TimerThread to stop, returns immediately.
     *
     * @details This will request the thread to stop, it does not wait for it to stop.
     */
    void request_stop() { thread_.request_stop(); }

   private:
    /**
     * Run the TimerThread, calling the callback after the given duration and repeating
     * until st.stop_requested() is true.
     *
     * @param st The stop_token to check for stop_requested().
     * @param callback The function to call each time the duration has elapsed
     * @param duration The periodic duration to wait before calling the callback
     */
    static void run(std::stop_token st,
                    CallbackType const& callback,
                    std::chrono::milliseconds duration);

   private:
    std::jthread thread_;
};

/**
 * Represents the terminal itself, providing an event loop and screen writing tools.
 */
class Terminal {
   public:
    using Cursor = std::optional<Point>;

   public:
    inline static ScreenBuffer changes{{0, 0}};  // write to this
    inline static EventQueue event_queue;        // read from this

    inline static std::map<int, TimerThread> timers;

    inline static Color foreground = TermColor::Default;
    inline static Color background = TermColor::Default;

    /**
     * The current cursor position on the terminal.
     *
     * @details This is used by the event loop after `changes` has been committed to the
     * terminal. If this is std::nullopt, then the cursor is not displayed.
     */
    inline static Cursor cursor{std::nullopt};

   public:
    /**
     * Initializes the terminal display and starts reading events in separate thread,
     * appending to the event_queue.
     *
     * @param mouse_mode The MouseMode to set the terminal to.
     * @param key_mode The KeyMode to set the terminal to.
     * @param signals Whether OS Signals should be enabled or disabled.
     */
    Terminal(MouseMode mouse_mode = MouseMode::Basic,
             KeyMode key_mode = KeyMode::Normal,
             Signals signals = Signals::On);

    Terminal(Terminal&&) = default;
    auto operator=(Terminal&&) -> Terminal& = default;

    /**
     * Uninitializes the terminal display, resets the state to before the constructor
     * was called.
     */
    ~Terminal();

   public:
    /**
     * Write changes ScreenBuffer to the terminal and update current_screen_.
     *
     * @details This is called automatically by the Application class after an event has
     * been processed.
     */
    void commit_changes();

    /**
     * Runs a loop that reads input from the terminal and appends it to the
     * Terminal::event_queue. Exits when the stop_token is stop_requested().
     *
     * @param st The stop_token to check for stop_requested().
     */
    void run_read_loop(std::stop_token st);

    /**
     * Return the current size of the terminal.
     *
     * @return Area The current dimensions of the terminal.
     */
    [[nodiscard]] auto size() -> Area;

   private:
    ScreenBuffer current_screen_{{0, 0}};
    std::jthread terminal_input_thread_;
    std::string escape_sequence_;
};

/**
 * A handle to a Timer in the Terminal's timer system.
 *
 * @details This is used to create a new TimerThread with a given ID and provides access
 * to start and stop it. The thread will post Timer events to the event queue at the
 * given interval.
 */
class Timer {
   public:
    /**
     * Create a Timer with the given duration.
     *
     * @param duration The periodic duration to wait before calling the callback
     * @param launch If true, the TimerThread will be started immediately.
     */
    explicit Timer(std::chrono::milliseconds duration, bool launch = false);

    Timer(Timer const&) = delete;
    Timer(Timer&& other);

    auto operator=(Timer const&) -> Timer& = delete;
    auto operator=(Timer&& other) -> Timer&;

    /**
     * Stop the TimerThread if it is running.
     */
    ~Timer();

   public:
    /**
     * Start the timer thread with the given duration.
     *
     * @details This will create and launch a new thread.
     */
    void start();

    /**
     * Request the TimerThread to stop, returns immediately.
     *
     * @details Does not wait for thread to exit. The destructor of Terminal will wait
     * for it to exit, or if you call start() again.
     */
    void stop();

    [[nodiscard]] auto id() const -> int { return id_; }

    [[nodiscard]] auto duration() const -> std::chrono::milliseconds
    {
        return duration_;
    }

    [[nodiscard]] auto is_running() const -> bool { return is_running_; }

   private:
    inline static int next_id_ = 0;

    int id_;
    std::chrono::milliseconds duration_;
    bool is_running_ = false;
};

/**
 * A 2D Rectangle that represents a paintable region on the terminal.
 *
 * @details The `at` member is the top left position of the Canvas in the global
 * terminal space.
 */
struct Canvas {
    ScreenBuffer& buffer;
    Point at;
    Area size;

    /**
     * Provides mutable access to the given Point of the Canvas.
     *
     * @param p The Point position of the Glyph in the Canvas. There is no bounds
     * checking, the point should be in the range [{0, 0}, size).
     */
    [[nodiscard]] auto operator[](Point p) -> Glyph&;

    /**
     * Provides const access to the given Point of the Canvas.
     *
     * @param p The Point position of the Glyph in the Canvas. There is no bounds
     * checking, the point should be in the range [{0, 0}, size).
     */
    [[nodiscard]] auto operator[](Point p) const -> Glyph const&;
};

/**
 * Calls the appropriate handler function on \p handler for the given Event.
 *
 * @details This will only call the handler if the handler has the appropriate function
 * for the given Event type.
 * @param ev The Event to handle.
 * @param handler The event handler to process the event.
 * @return std::optional<EventResponse> The response from the handler, or std::nullopt
 * if not handled.
 */
template <typename T>
[[nodiscard]] auto apply_event(Event const& ev, T& handler)
    -> std::optional<EventResponse>
{
    return std::visit(
        zzz::Overload{[&](esc::KeyPress e) -> std::optional<EventResponse> {
                          if constexpr (HandlesKeyPress<T>) {
                              return handler.handle_key_press(e.key);
                          }
                          else {
                              return std::nullopt;
                          }
                      },
                      [&](esc::KeyRelease e) -> std::optional<EventResponse> {
                          if constexpr (HandlesKeyRelease<T>) {
                              return handler.handle_key_release(e.key);
                          }
                          else {
                              return std::nullopt;
                          }
                      },
                      [&](esc::MousePress e) -> std::optional<EventResponse> {
                          if constexpr (HandlesMousePress<T>) {
                              return handler.handle_mouse_press(e.mouse);
                          }
                          else {
                              return std::nullopt;
                          }
                      },
                      [&](esc::MouseRelease e) -> std::optional<EventResponse> {
                          if constexpr (HandlesMouseRelease<T>) {
                              return handler.handle_mouse_release(e.mouse);
                          }
                          else {
                              return std::nullopt;
                          }
                      },
                      [&](esc::MouseWheel e) -> std::optional<EventResponse> {
                          if constexpr (HandlesMouseWheel<T>) {
                              return handler.handle_mouse_wheel(e.mouse);
                          }
                          else {
                              return std::nullopt;
                          }
                      },
                      [&](esc::MouseMove e) -> std::optional<EventResponse> {
                          if constexpr (HandlesMouseMove<T>) {
                              return handler.handle_mouse_move(e.mouse);
                          }
                          else {
                              return std::nullopt;
                          }
                      },
                      [&](esc::Resize e) -> std::optional<EventResponse> {
                          Terminal::changes.resize(e.size);
                          Terminal::changes.fill(Glyph{});
                          if constexpr (HandlesResize<T>) {
                              return handler.handle_resize(e.size);
                          }
                          else {
                              return std::nullopt;
                          }
                      },
                      [&](event::Timer e) -> std::optional<EventResponse> {
                          if constexpr (HandlesTimer<T>) {
                              return handler.handle_timer(e.id);
                          }
                          else {
                              return std::nullopt;
                          }
                      },
                      [](event::Custom const& e) -> std::optional<EventResponse> {
                          return e.action();
                      },
                      [](event::Interrupt) -> std::optional<EventResponse> {
                          return QuitRequest{1};
                      }},
        ev);
}

template <typename T>
concept HandlesPaint = requires(T t, Canvas c) {
    { t.handle_paint(c) } -> std::same_as<Terminal::Cursor>;
};

/**
 * Runs an event loop over the Terminal::event_queue, sending events to the given event
 * handler.
 *
 * @details This will block until it receives a QuitRequest. The application can be quit
 * by responding to an Event handler with a QuitRequest object.
 * @param term The Terminal object.
 * @param handler The handler object that all events will be sent to.
 * @return The return code of the application, passed in via QuitRequest.
 */
template <typename EventHandler>
[[nodiscard]] auto process_events(Terminal& term, EventHandler& handler) -> int
{
    while (true) {
        auto const event = Terminal::event_queue.pop();  // Blocking Call
        auto const result = apply_event(event, handler);

        if (result.has_value()) {
            if (auto& quit = *result; quit.has_value()) { return quit->return_code; }
            else {
                if constexpr (HandlesPaint<EventHandler>) {
                    term.cursor = handler.handle_paint(Canvas{
                        .buffer = Terminal::changes,
                        .at = {0, 0},
                        .size = Terminal::changes.size(),
                    });
                }
                term.commit_changes();
            }
        }
    }
}

}  // namespace ox
#pragma once

#include <algorithm>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <functional>
#include <map>
#include <optional>
#include <stop_token>
#include <string>
#include <thread>
#include <vector>

#include <esc/area.hpp>
#include <esc/detail/signals.hpp>
#include <esc/event.hpp>
#include <esc/io.hpp>
#include <esc/point.hpp>
#include <esc/sequence.hpp>
#include <esc/terminal.hpp>

#include <termox/common.hpp>
#include <termox/events.hpp>
#include <termox/glyph.hpp>

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
     * @param area The dimensions of the ScreenBuffer.
     */
    explicit ScreenBuffer(Area area);

   public:
    /**
     * Access the Glyph at the given position.
     *
     * @details The top left is `{0, 0}` and the bottom right is `{width - 1,
     * height - 1}`. Does no bounds checking.
     * @param p The Point position of the Glyph.
     * @return Glyph& A reference to the Glyph at the given position.
     */
    [[nodiscard]] auto operator[](Point p) -> Glyph&;

    /**
     * Access the Glyph at the given position.
     *
     * @details The top left is `{0, 0}` and the bottom right is `{width - 1,
     * height - 1}`. Does no bounds checking.
     * @param p The Point position of the Glyph.
     * @return Glyph const& A reference to the Glyph at the given position.
     */
    [[nodiscard]] auto operator[](Point p) const -> Glyph const&;

    /**
     * Resize the ScreenBuffer to the given dimensions and clears state.
     *
     * @details This does not preserve the contents of the ScreenBuffer, it
     * overwrites all Glyph symbols with null bytes.
     * @details This function is provided as an optimized alternative to
     * constructing a new ScreenBuffer and moving it into the previous one. This
     * minimizes the vector allocations by re-using the existing buffer.
     * @param a The new dimensions of the ScreenBuffer.
     */
    auto reset(Area a) -> void;

    /**
     * Clear the ScreenBuffer, setting all Glyphs symbols to null.
     *
     * @details This does not reset their Brush members, a null symbol is
     * 'empty'.
     */
    auto clear() -> void;

    /**
     * Return the dimensions of the ScreenBuffer.
     *
     * @return Area The dimensions of the ScreenBuffer.
     */
    [[nodiscard]] auto area() const -> Area { return area_; }

   private:
    Area area_;
    std::vector<Glyph> buffer_;
};

/**
 * A thread of execution that waits for a given duration and then calls a
 * callback function.
 */
class TimerThread {
   public:
    using CallbackType = std::function<void()>;
    using ClockType    = std::chrono::steady_clock;

   public:
    /**
     * Create a placeholder TimerThread that does nothing.
     *
     * @details You'll want to move assign over this to launch a TimerThread.
     */
    TimerThread() = default;

    /**
     * Create a TimerThread that will call the given callback after the given
     * duration.
     *
     * @details This will not launch a thread, you need to call start() to do
     * that.
     * @param duration The periodic duration to wait before calling the callback
     * @param callback The function to call each time the duration has elapsed
     */
    explicit TimerThread(std::chrono::milliseconds duration,
                         CallbackType callback);

    TimerThread(TimerThread const&) = delete;
    TimerThread(TimerThread&&)      = default;

    auto operator=(TimerThread const&) -> TimerThread& = delete;
    auto operator=(TimerThread&& x) -> TimerThread&    = default;

   public:
    /**
     * Request the TimerThread to stop, returns immediately.
     *
     * @details This will request the thread to stop, it does not wait for it to
     * stop.
     */
    auto request_stop() -> void { thread_.request_stop(); }

   private:
    /**
     * Run the TimerThread, calling the callback after the given duration and
     * repeating until st.stop_requested() is true.
     *
     * @param st The stop_token to check for stop_requested().
     * @param callback The function to call each time the duration has elapsed
     * @param duration The periodic duration to wait before calling the callback
     */
    static auto run(std::stop_token st,
                    CallbackType const& callback,
                    std::chrono::milliseconds duration) -> void;

   private:
    std::jthread thread_;
};

/**
 * Represents the terminal itself, providing an event loop and screen writing
 * tools.
 */
class Terminal {
   public:
    inline static ScreenBuffer changes{{0, 0}};  // write to this
    inline static EventQueue event_queue;        // read from this

    inline static std::map<int, TimerThread> timers;

    /**
     * The current cursor position on the terminal.
     *
     * @details This is used by the event loop after `changes` has been
     * committed to the terminal. If this is std::nullopt, then the cursor is
     * not displayed.
     */
    inline static std::optional<Point> cursor{std::nullopt};

   public:
    /**
     * Initializes the terminal display and starts reading events in separate
     * thread, appending to the event_queue.
     *
     * @param mouse_mode The MouseMode to set the terminal to.
     * @param key_mode The KeyMode to set the terminal to.
     * @param signals Whether OS Signals should be enabled or disabled.
     */
    explicit Terminal(MouseMode mouse_mode = MouseMode::Basic,
                      KeyMode key_mode     = KeyMode::Normal,
                      Signals signals      = Signals::On);

    /**
     * Uninitializes the terminal display, resets the state to before the
     * constructor was called.
     */
    ~Terminal();

   public:
    /**
     * Write changes ScreenBuffer to the terminal and update current_screen_.
     *
     * @details This is called automatically by the Application class after an
     * event has been processed.
     */
    auto commit_changes() -> void;

    /**
     * Runs a loop that reads input from the terminal and appends it to the
     * Terminal::event_queue. Exits when the stop_token is stop_requested().
     *
     * @param st The stop_token to check for stop_requested().
     */
    auto run_read_loop(std::stop_token st) -> void;

    /**
     * Return the current dimensions of the terminal.
     *
     * @return Area The current dimensions of the terminal.
     */
    [[nodiscard]] auto area() -> Area;

   private:
    ScreenBuffer current_screen_{{0, 0}};
    std::jthread terminal_input_thread_;
    std::string escape_sequence_;
};

/**
 * A handle to a Timer in the Terminal's timer system.
 *
 * @details This is used to create a new TimerThread with a given ID and
 * provides access to start and stop it.
 */
class Timer {
   public:
    /**
     * Create a Timer with the given duration.
     *
     * @details This does not start the timer, you must call start() on it.
     * @param duration The periodic duration to wait before calling the callback
     */
    explicit Timer(std::chrono::milliseconds duration);

   public:
    /**
     * Start the timer thread with the given duration.
     *
     * @details This will create and launch a new thread.
     */
    auto start() -> void;

    /**
     * Request the TimerThread to stop, returns immediately.
     *
     * @details Does not wait for thread to exit. The destructor of Terminal
     * will wait for it to exit, or if you call start() again.
     */
    auto stop() -> void;

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
 * A 2D Rectangle that represents a paintable area on the terminal with location
 * and size.
 */
template <typename T>
concept Canvas = requires(T t) {
    {
        t.coordinates
    } -> std::same_as<Point&>;
    {
        t.size
    } -> std::same_as<Area&>;
};

/**
 * Provides a way to draw to the terminal screen.
 *
 * @details This writes directly to the global ScreenBuffer. It can be created
 * from anywhere; if constructed with a Canvas type it will constrain the
 * drawing to the Canvas' coordinates and size.
 */
class Painter {
    /**
     * Proxy object to write to the screen.
     *
     * @details This is created by Painter::operator[](Point) and allows stream
     * like chained insertion of Glyphs and strings. Any output starts at the
     * Point passed into Painter::operator[] and moves one cell to the right per
     * Glyph, until the bounds are reached.
     */
    class CursorWriter {
       public:
        explicit CursorWriter(Point at, Area bounds, ScreenBuffer& buffer);

       public:
        auto operator<<(Glyph const& g) && -> CursorWriter;

        auto operator<<(Glyph const& g) & -> CursorWriter&;

        template <Character T>
        auto operator<<(T c) && -> CursorWriter
        {
            return std::move(*this) << Glyph{
                       .symbol = static_cast<char32_t>(c),
                   };
        }

        template <Character T>
        auto operator<<(T c) & -> CursorWriter&
        {
            return *this << Glyph{
                       .symbol = static_cast<char32_t>(c),
                   };
        }

        template <GlyphString T>
        auto operator<<(T const& gs) && -> CursorWriter
        {
            auto const end =
                std::min(gs.size(), (std::size_t)(bounds_.width - at_.x));
            for (auto i = std::size_t{0}; i < end; ++i) {
                buffer_[at_] = gs[i];
                ++at_.x;
            }
            return std::move(*this);
        }

        template <GlyphString T>
        auto operator<<(T const& gs) & -> CursorWriter&
        {
            auto const end =
                std::min(gs.size(), (std::size_t)(bounds_.width - at_.x));
            for (auto i = std::size_t{0}; i < end; ++i) {
                buffer_[at_] = gs[i];
                ++at_.x;
            }
            return *this;
        }

        auto operator<<(std::string_view sv) && -> CursorWriter;

        auto operator<<(std::string_view sv) & -> CursorWriter&;

        auto operator<<(std::u32string_view sv) && -> CursorWriter;

        auto operator<<(std::u32string_view sv) & -> CursorWriter&;

       private:
        Point at_;
        Area bounds_;
        ScreenBuffer& buffer_;
    };

   public:
    /**
     * Construct a Painter for the entire terminal screen.
     */
    Painter();

    /**
     * Construct a Painter with the given Canvas' coordinates and size.
     *
     * @param canvas The Canvas to create a Painter for.
     */
    template <Canvas T>
    explicit Painter(T& canvas)
        : offset_{canvas.coordinates},
          size_{canvas.size},
          screen_{Terminal::changes}
    {}

   public:
    /**
     * Return a CursorWriter ready to write at the given Point.
     *
     * @details The top left is {0, 0} and the bottom right is {width - 1,
     * height - 1}. If the given Point is out of bounds, writing will be a
     * no-op.
     * @param p The Point on the Canvas to start writing at.
     * @return CursorWriter
     */
    [[nodiscard]] auto operator[](Point p) -> CursorWriter;

    /**
     * Clear the underlying Canvas, setting all Glyphs symbols to null.
     *
     * @details This does not reset their Brush members, a null symbol is
     * 'empty'.
     */
    auto clear() -> void;

    /**
     * Fill the underlying Canvas with the given Glyph.
     *
     * @param g The Glyph to fill the Canvas with.
     */
    auto fill(Glyph const& g) -> void;

   private:
    Point offset_;
    Area size_;
    ScreenBuffer& screen_;
};

/**
 * Calls the appropriate handler function on \p handler for the given Event.
 *
 * @details This will only call the handler if the handler has the appropriate
 * function for the given Event type.
 * @param ev The Event to handle.
 * @param handler The event handler to process the event.
 * @return EventResponse The response from the handler.
 */
template <typename T>
[[nodiscard]] auto apply_event(Event const& ev, T& handler)
    -> std::optional<EventResponse>
{
    return std::visit(
        Overload{
            [&](esc::KeyPress e) -> EventResponse {
                if constexpr (HandlesKeyPress<T>) {
                    return handler.handle_key_press(e.key);
                }
                else {
                    return std::nullopt;
                }
            },
            [&](esc::KeyRelease e) -> EventResponse {
                if constexpr (HandlesKeyRelease<T>) {
                    return handler.handle_key_release(e.key);
                }
                else {
                    return std::nullopt;
                }
            },
            [&](esc::MousePress e) -> EventResponse {
                if constexpr (HandlesMousePress<T>) {
                    return handler.handle_mouse_press(e.mouse);
                }
                else {
                    return std::nullopt;
                }
            },
            [&](esc::MouseRelease e) -> EventResponse {
                if constexpr (HandlesMouseRelease<T>) {
                    return handler.handle_mouse_release(e.mouse);
                }
                else {
                    return std::nullopt;
                }
            },
            [&](esc::MouseWheel e) -> EventResponse {
                if constexpr (HandlesMouseWheel<T>) {
                    return handler.handle_mouse_wheel(e.mouse);
                }
                else {
                    return std::nullopt;
                }
            },
            [&](esc::MouseMove e) -> EventResponse {
                if constexpr (HandlesMouseMove<T>) {
                    return handler.handle_mouse_move(e.mouse);
                }
                else {
                    return std::nullopt;
                }
            },
            [&](esc::Resize e) -> EventResponse {
                Terminal::changes.reset(e.area);
                if constexpr (HandlesResize<T>) {
                    return handler.handle_resize(e.area);
                }
                else {
                    return std::nullopt;
                }
            },
            [&](event::Timer e) -> EventResponse {
                if constexpr (HandlesTimer<T>) {
                    return handler.handle_timer(e.id);
                }
                else {
                    return std::nullopt;
                }
            },
            [](event::Custom const& e) -> EventResponse { return e.action(); },
            [](event::Interrupt) -> EventResponse { return QuitRequest{1}; }},
        ev);
}

/**
 * Runs an event loop over the Terminal::event_queue, sending events to the
 * given event handler.
 *
 * @details This will block until it receives a QuitRequest. The application can
 * be quit by responding to an Event handler with a QuitRequest object.
 * @param term The Terminal object.
 * @param handler The handler object that all events will be sent to.
 * @return The return code of the application, passed in via QuitRequest.
 */
template <typename EventHandler>
[[nodiscard]] auto process_events(Terminal& term, EventHandler& handler) -> int
{
    while (true) {
        auto const event  = Terminal::event_queue.pop();  // Blocking Call
        auto const result = apply_event(event, handler);

        if (result.has_value()) {
            if (auto& quit = *result; quit.has_value()) {
                return quit->return_code;
            }
            else {
                term.commit_changes();
            }
        }
    }
}

}  // namespace ox
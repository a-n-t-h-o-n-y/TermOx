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
 * @brief A 2D Matrix of Glyphs that represents a paintable screen.
 */
class ScreenBuffer {
   public:
    /**
     * @brief Construct a ScreenBuffer with the given dimensions.
     *
     * @param area The dimensions of the ScreenBuffer.
     */
    explicit ScreenBuffer(Area area)
        : area_{area}, buffer_((std::size_t)(area.width * area.height))
    {}

   public:
    /**
     * @brief Access the Glyph at the given position.
     *
     * The top left is {0, 0} and the bottom right is {width - 1, height - 1}.
     * Does no bounds checking.
     *
     * @param p The Point position of the Glyph.
     * @return Glyph& A reference to the Glyph at the given position.
     */
    [[nodiscard]] auto operator[](Point p) -> Glyph&
    {
        return buffer_[(std::size_t)(p.y * area_.width + p.x)];
    }

    /**
     * @brief Access the Glyph at the given position.
     *
     * The top left is {0, 0} and the bottom right is {width - 1, height - 1}.
     * Does no bounds checking.
     *
     * @param p The Point position of the Glyph.
     * @return Glyph const& A reference to the Glyph at the given position.
     */
    [[nodiscard]] auto operator[](Point p) const -> Glyph const&
    {
        return buffer_[(std::size_t)(p.y * area_.width + p.x)];
    }

    /**
     * @brief Resize the ScreenBuffer to the given dimensions and clears state.
     *
     * This does not preserve the contents of the ScreenBuffer, it overwrites
     * all Glyph symbols with null bytes.
     *
     * This function is provided as an optimized alternative to constructing
     * a new ScreenBuffer and moving it into the previous one. This minimizes
     * the vector allocations by re-using the existing buffer.
     *
     * @param a The new dimensions of the ScreenBuffer.
     */
    auto reset(Area a) -> void
    {
        area_ = a;
        buffer_.resize((std::size_t)(a.width * a.height));
        this->clear();
    }

    /**
     * @brief Clear the ScreenBuffer, setting all Glyphs symbols to null.
     *
     * This does not reset their Brush members, a null symbol is 'empty'.
     */
    auto clear() -> void
    {
        for (auto& g : buffer_) {
            g.symbol = U'\0';
        }
    }

    /**
     * @brief Return the dimensions of the ScreenBuffer.
     *
     * @return Area The dimensions of the ScreenBuffer.
     */
    [[nodiscard]] auto area() const -> Area { return area_; }

   private:
    Area area_;
    std::vector<Glyph> buffer_;
};

/**
 * @brief A thread of execution that waits for a given duration and then calls
 * a callback function.
 */
class TimerThread {
   public:
    using CallbackType = std::function<void()>;
    using ClockType    = std::chrono::steady_clock;

   public:
    /**
     * @brief Create a placeholder TimerThread that does nothing.
     *
     * You'll want to move assign over this to launch a TimerThread.
     */
    TimerThread() = default;

    /**
     * @brief Create a TimerThread that will call the given callback after the
     * given duration.
     *
     * This will not launch a thread, you need to call start() to do that.
     *
     * @param duration The periodic duration to wait before calling the callback
     * @param callback The function to call each time the duration has elapsed
     */
    explicit TimerThread(std::chrono::milliseconds duration,
                         CallbackType callback)
        : thread_{[cb = std::move(callback), d = std::move(duration)](auto st) {
              TimerThread::run(st, cb, d);
          }}
    {}

    TimerThread(TimerThread const&) = delete;
    TimerThread(TimerThread&&)      = default;

    auto operator=(TimerThread const&) -> TimerThread& = delete;
    auto operator=(TimerThread&& x) -> TimerThread&    = default;

   public:
    /**
     * @brief Request the TimerThread to stop, returns immediately.
     *
     * This will request the thread to stop, it does not wait for it to stop.
     */
    auto request_stop() -> void { thread_.request_stop(); }

   private:
    /**
     * @brief Run the TimerThread, calling the callback after the given
     * duration and repeating until st.stop_requested() is true.
     *
     * @param st The stop_token to check for stop_requested().
     * @param callback The function to call each time the duration has elapsed
     * @param duration The periodic duration to wait before calling the callback
     */
    static void run(std::stop_token st,
                    CallbackType const& callback,
                    std::chrono::milliseconds duration)
    {
        constexpr auto timeout_duration =
            std::chrono::duration_cast<ClockType::duration>(
                std::chrono::milliseconds{16});
        auto next_callback_time = ClockType::now() + duration;
        while (true) {
            if (st.stop_requested()) {
                return;
            }

            auto const now = ClockType::now();
            if (now >= next_callback_time) {
                callback();
                next_callback_time += duration;
            }

            auto const time_to_wait =
                std::min(next_callback_time - now, timeout_duration);
            std::this_thread::sleep_for(time_to_wait);
        }
    }

   private:
    std::jthread thread_;
};

class Terminal {
   public:
    inline static ScreenBuffer changes{{0, 0}};  // write to this
    inline static EventQueue event_queue;        // read from this

    inline static std::map<int, TimerThread> timers;

    /**
     * @brief The current cursor position on the terminal.
     *
     * This is used by the event loop after `changes` has been committed to the
     * terminal. If this is std::nullopt, then the cursor is not displayed.
     */
    inline static std::optional<Point> cursor{std::nullopt};

   public:
    /**
     * @brief Initializes the terminal display and starts reading events in
     * separate thread, appending to the event_queue.
     *
     * @param mouse_mode The MouseMode to set the terminal to.
     * @param key_mode The KeyMode to set the terminal to.
     * @param signals Whether OS Signals should be enabled or disabled.
     */
    explicit Terminal(MouseMode mouse_mode = MouseMode::Basic,
                      KeyMode key_mode     = KeyMode::Normal,
                      Signals signals      = Signals::On)
        : terminal_input_thread_{[this](auto st) { this->run_read_loop(st); }}
    {
        esc::initialize_interactive_terminal(mouse_mode, key_mode, signals);
    }

    /**
     * @brief Uninitializes the terminal display.
     */
    ~Terminal()
    {
        terminal_input_thread_.request_stop();
        esc::uninitialize_terminal();
    }

   public:
    /**
     * @brief Write changes ScreenBuffer to the terminal and update
     * current_screen_.
     *
     * This is called automatically by the Application class after an event has
     * been processed.
     */
    auto commit_changes() -> void
    {
        escape_sequence_.clear();

        if (Terminal::changes.area() != current_screen_.area()) {
            current_screen_.reset(Terminal::changes.area());
        }

        for (auto x = 0; x < changes.area().width; ++x) {
            for (auto y = 0; y < changes.area().height; ++y) {
                auto const& change  = changes[{x, y}];
                auto const& current = current_screen_[{x, y}];
                if (change.symbol != U'\0' && change != current) {
                    escape_sequence_ += escape(esc::Cursor_position{x, y});
                    escape_sequence_ += escape(change);
                    current_screen_[{x, y}] = change;
                }
            }
        }

        esc::write(escape_sequence_);

        if (cursor.has_value()) {
            set(esc::Cursor::Show);
            esc::write(escape(esc::Cursor_position{*cursor}));
        }
        else {
            esc::set(esc::Cursor::Hide);
        }

        esc::flush();
    }

    /**
     * @brief Runs a loop that reads input from the terminal and appends it to
     * the Terminal::event_queue. Exits when the stop_token is stop_requested().
     *
     * @param st The stop_token to check for stop_requested().
     */
    auto run_read_loop(std::stop_token st) -> void
    {
        Terminal::event_queue.append(esc::Resize{Terminal::area()});

        while (!st.stop_requested()) {
            if (esc::sigint_flag == 1) {
                Terminal::event_queue.append(event::Interrupt{});
                return;
            }
            else if (auto const event = esc::read(16); event.has_value()) {
                Terminal::event_queue.append(std::visit(
                    [](auto const& e) -> Event { return e; }, *event));
                // ^^ Translate from esc::Event to ox::Event ^^
            }
        }
    }

    /**
     * @brief Return the current dimensions of the terminal.
     *
     * @return Area The current dimensions of the terminal.
     */
    [[nodiscard]] auto area() -> Area { return esc::terminal_area(); }

   private:
    ScreenBuffer current_screen_{{0, 0}};
    std::jthread terminal_input_thread_;
    std::string escape_sequence_;
};

/**
 * @brief A handle to a Timer in the Terminal's timer system.
 *
 * This is used to create a new TimerThread with a given ID and provides access
 * to start and stop it.
 */
class Timer {
   public:
    /**
     * @brief Create a Timer with the given duration.
     *
     * This does not start the timer, you must call start() on it.
     *
     * @param duration The periodic duration to wait before calling the callback
     */
    explicit Timer(std::chrono::milliseconds duration)
        : id_{next_id_++}, duration_{duration}
    {
        Terminal::timers.emplace(id_, TimerThread{});
    }

   public:
    /**
     * @brief Start the timer thread with the given duration.
     *
     * This will create and launch a new thread.
     */
    auto start() -> void
    {
        Terminal::timers[id_] = TimerThread{
            duration_,
            [id = id_] { Terminal::event_queue.append(event::Timer{id}); }};
        is_running_ = true;
    }

    /**
     * @brief Request the TimerThread to stop, returns immediately.
     *
     * Does not wait for thread to exit. The destructor of Terminal will wait
     * for it to exit, or if you call start() again.
     */
    auto stop() -> void
    {
        Terminal::timers[id_].request_stop();
        is_running_ = false;
    }

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
 * @brief A 2D Rectangle that represents a paintable area on the terminal with
 * location and size.
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
        explicit CursorWriter(Point at, Area bounds, ScreenBuffer& buffer)
            : at_{at}, bounds_{bounds}, buffer_{buffer}
        {}

       public:
        auto operator<<(Glyph const& g) && -> CursorWriter
        {
            if (at_.x < bounds_.width && at_.y < bounds_.height) {
                buffer_[at_] = g;
                ++at_.x;
            }
            return std::move(*this);
        }

        auto operator<<(Glyph const& g) & -> CursorWriter&
        {
            if (at_.x < bounds_.width && at_.y < bounds_.height) {
                buffer_[at_] = g;
                ++at_.x;
            }
            return *this;
        }

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

        auto operator<<(std::string_view sv) && -> CursorWriter
        {
            return std::move(*this) << esc::detail::utf8_to_glyphs(sv);
        }

        auto operator<<(std::string_view sv) & -> CursorWriter&
        {
            return *this << esc::detail::utf8_to_glyphs(sv);
        }

        auto operator<<(std::u32string_view sv) && -> CursorWriter
        {
            return std::move(*this) << esc::detail::utf32_to_glyphs(sv);
        }

        auto operator<<(std::u32string_view sv) & -> CursorWriter&
        {
            return *this << esc::detail::utf32_to_glyphs(sv);
        }

       private:
        Point at_;
        Area bounds_;
        ScreenBuffer& buffer_;
    };

   public:
    /**
     * @brief Construct a Painter for the entire terminal screen.
     */
    Painter()
        : offset_{0, 0},
          size_{Terminal::changes.area()},
          screen_{Terminal::changes}
    {}

    /**
     * @brief Construct a Painter with the given Canvas' coordinates and size.
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
    // TODO these should return a CursorWriter. Maybe remove the const versions.

    /**
     * @brief Access the Glyph at the given position, offset for Canvas.
     *
     * The top left is {0, 0} and the bottom right is {width - 1, height - 1}.
     * Does no bounds checking.
     *
     * @param p The Point position of the Glyph.
     * @return Glyph& A reference to the Glyph at the given position.
     */
    [[nodiscard]] auto operator[](Point p) -> CursorWriter
    {
        return CursorWriter{
            {
                .x = p.x + offset_.x,
                .y = p.y + offset_.y,
            },
            size_,
            screen_,
        };
    }

    /**
     * @brief Clear the underlying Canvas, setting all Glyphs symbols to null.
     *
     * This does not reset their Brush members, a null symbol is 'empty'.
     */
    auto clear() -> void
    {
        for (auto x = offset_.x; x < offset_.x + size_.width; ++x) {
            for (auto y = offset_.y; y < offset_.y + size_.height; ++y) {
                screen_[{x, y}].symbol = U'\0';
            }
        }
    }

    /**
     * @brief Fill the underlying Canvas with the given Glyph.
     *
     * @param g The Glyph to fill the Canvas with.
     */
    auto fill(Glyph const& g) -> void
    {
        for (auto x = offset_.x; x < offset_.x + size_.width; ++x) {
            for (auto y = offset_.y; y < offset_.y + size_.height; ++y) {
                screen_[{x, y}] = g;
            }
        }
    }

    // TODO maybe a border() drawing function? You'll have to handle smaller
    // dimensions yourself in the widget then.

   private:
    Point offset_;
    Area size_;
    ScreenBuffer& screen_;
};

/**
 * @brief Calls the appropriate handler function on \p handler for the given
 * Event.
 *
 * This will only call the handler if the handler has the appropriate function
 * for the given Event type.
 *
 * @param ev The Event to handle.
 * @param handler The event handler to process the event.
 * @return EventResponse The response from the handler.
 */
template <typename T>
[[nodiscard]] auto apply_event(Event const& ev, T& handler) -> EventResponse
{
    // TODO can this return an optional event response? so if the first layer is
    // null then you do nothing, if the second layer is null you commit changes
    // and if the second is not null then you quit. An optimization for when
    // events have no implementaion.
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
 * @brief Runs an event loop over the Terminal::event_queue, sending events to
 * the given event handler.
 *
 * This will block until it receives a QuitRequest. The application can be
 * quit by responding to an Event handler with a QuitRequest object.
 *
 * @param term The Terminal object.
 * @param handler The handler object that all events will be sent to.
 * @return The return code of the application, passed in via QuitRequest.
 */
template <typename EventHandler>
[[nodiscard]] auto process_events(Terminal& term, EventHandler& handler) -> int
{
    while (true) {
        auto const event = Terminal::event_queue.pop();  // Blocking Call
        auto const quit  = apply_event(event, handler);

        if (quit.has_value()) {
            // TODO add extra optional layer to apply_event response
            return quit->return_code;
        }
        else {
            term.commit_changes();
        }
    }
}

}  // namespace ox
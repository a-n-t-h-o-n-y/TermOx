#pragma once

#include <chrono>
#include <map>
#include <optional>
#include <string>
#include <type_traits>
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
 * Represents the terminal itself, providing an event loop and screen writing tools.
 */
class Terminal {
   public:
    using Cursor = std::optional<Point>;

    struct Options {
        MouseMode mouse_mode = MouseMode::Basic;
        Signals signals = Signals::Off;
        Color foreground = default_color;
        Color background = default_color;
    };

   public:
    Color foreground = default_color;
    Color background = default_color;

   public:
    /**
     * Initializes the terminal display.
     */
    Terminal(Options x);

    /**
     * Initializes the terminal display.
     *
     * @param mouse_mode The MouseMode to set the terminal to.
     * @param signals Whether OS Signals should be enabled or disabled.
     * @param foreground The default foreground for the Terminal.
     * @param background The default background for the Terminal.
     */
    Terminal(MouseMode mouse_mode = MouseMode::Basic,
             Signals signals = Signals::Off,
             Color foreground_ = default_color,
             Color background_ = default_color);

    Terminal(Terminal&&) = delete;
    auto operator=(Terminal&&) -> Terminal& = delete;

    /**
     * Uninitializes the terminal display, resets the state to before the constructor
     * was called.
     */
    ~Terminal() = default;

   public:
    /**
     * Compare the supplied screen with the previous frame and write changes.
     *
     * @details This is called automatically by the Application class after an event has
     * been processed.
     */
    void commit_changes(ScreenBuffer const& screen, Cursor cursor = {});

    /** Read terminal input on the calling thread. */
    auto read(std::chrono::milliseconds timeout) -> std::optional<Event>;

    /**
     * Returns the current size of the terminal.
     *
     * @return Area The current dimensions of the terminal.
     */
    [[nodiscard]] auto size() -> Area;

   private:
    ScreenBuffer current_screen_{{0, 0}};
    esc::Terminal session_;
    std::string escape_sequence_;
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
 * @param buffer The ScreenBuffer to write changes to on paint event.
 * @return Handling, repaint, and exit decisions.
 */
template <typename T>
[[nodiscard]] auto apply_event(Event const& ev, T& handler, ScreenBuffer& buffer)
    -> EventResponse
{
    return std::visit(
        zzz::Overload{
            [&](esc::KeyEvent const& e) -> EventResponse {
                if (e.action == KeyAction::Release) {
                    if constexpr (HandlesKeyRelease<T>) {
                        return handler.handle_key_release(e);
                    }
                }
                else {
                    if constexpr (HandlesKeyPress<T>) {
                        return handler.handle_key_press(e);
                    }
                }
                return {};
            },
            [&](esc::MousePress e) -> EventResponse {
                if constexpr (HandlesMousePress<T>) {
                    return handler.handle_mouse_press(e);
                }
                else {
                    return {};
                }
            },
            [&](esc::MouseRelease e) -> EventResponse {
                if constexpr (HandlesMouseRelease<T>) {
                    return handler.handle_mouse_release(e);
                }
                else {
                    return {};
                }
            },
            [&](esc::MouseWheel e) -> EventResponse {
                if constexpr (HandlesMouseWheel<T>) {
                    return handler.handle_mouse_wheel(e);
                }
                else {
                    return {};
                }
            },
            [&](esc::MouseMove e) -> EventResponse {
                if constexpr (HandlesMouseMove<T>) {
                    return handler.handle_mouse_move(e);
                }
                else {
                    return {};
                }
            },
            [&](esc::Resize e) -> EventResponse {
                buffer.resize(e.size);
                buffer.fill(Glyph{});
                if constexpr (HandlesResize<T>) {
                    auto result = handler.handle_resize(e.size);
                    result.repaint = true;
                    return result;
                }
                else {
                    return {.repaint = true};
                }
            },
            [&](event::Timer e) -> EventResponse {
                if constexpr (HandlesTimer<T>) { return handler.handle_timer(e.id); }
                else {
                    return {};
                }
            },
            [](event::Custom const& e) -> EventResponse { return e.action(); },
            [](event::Interrupt) -> EventResponse {
                return {.handled = true, .quit = 1};
            },
            [&](esc::PasteChunk const& e) -> EventResponse {
                if constexpr (requires { handler.handle_paste(e); }) {
                    return handler.handle_paste(e);
                }
                return {};
            },
            [](auto const&) -> EventResponse { return {}; }},
        ev);
}

template <typename T>
concept HandlesPaint = requires(T t, Canvas c) {
    { t.handle_paint(c) } -> std::same_as<Terminal::Cursor>;
};

}  // namespace ox

/** @file
 * Terminal session lifetime, input, and changed-frame presentation.
 */
#pragma once

#include <chrono>
#include <optional>

#include <esc/terminal.hpp>

#include <ox/core/events.hpp>
#include <ox/core/frame.hpp>
#include <ox/core/renderer.hpp>

namespace ox {

using ::esc::MouseMode;
using ::esc::Signals;

/**
 * Owns a terminal session and presents frames through a renderer.
 */
class Terminal {
   public:
    /** Session input modes and fallback colors; field initializers define the defaults. */
    struct Options {
        /** Escape mouse-reporting mode enabled for this session. */
        MouseMode mouse_mode = MouseMode::Basic;
        /** Whether Escape manages operating-system signal handling. */
        Signals signals = Signals::Off;
        /** Fallback foreground for cells using the terminal default foreground. */
        Color foreground = default_color;
        /** Fallback background for cells using the terminal default background. */
        Color background = default_color;
    };

   public:
    /** Fallback foreground passed to the renderer; explicit cell foreground colors take
     * precedence.
     */
    Color foreground = default_color;
    /** Fallback background passed to the renderer; explicit cell background colors take
     * precedence.
     */
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
     * @param foreground_ The default foreground for the Terminal.
     * @param background_ The default background for the Terminal.
     */
    Terminal(MouseMode mouse_mode = MouseMode::Basic,
             Signals signals = Signals::Off,
             Color foreground_ = default_color,
             Color background_ = default_color);

    /** A live terminal session cannot move between owners. */
    Terminal(Terminal&&) = delete;
    /** Terminal sessions cannot be move-assigned. */
    auto operator=(Terminal&&) -> Terminal& = delete;

    /**
     * Uninitializes the terminal display, resets the state to before the constructor
     * was called.
     */
    ~Terminal() noexcept;

   public:
    /**
     * Compare the supplied screen with the previous frame and write changes.
     *
     * @details This is called automatically by the Application class after an event has
     * been processed.
     */
    void commit_changes(Frame const& screen, Cursor cursor = {});

    /** Borrow Escape readiness and deadlines for a host wait. */
    auto wait_state() -> esc::WaitState { return session_.wait_state(); }

    /** Read terminal input on the calling thread. */
    auto read(std::chrono::milliseconds timeout) -> std::optional<Event>;

    /**
     * Returns the current size of the terminal.
     *
     * @return Area The current dimensions of the terminal.
     */
    [[nodiscard]] auto size() -> Area;

   private:
    esc::Terminal session_;
    Renderer renderer_;
    bool cursor_visible_ = false;
};

}  // namespace ox

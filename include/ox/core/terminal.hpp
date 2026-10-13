#pragma once

#include <chrono>
#include <optional>
#include <string>

#include <esc/terminal.hpp>

#include <ox/core/events.hpp>
#include <ox/core/frame.hpp>

namespace ox {

using ::esc::MouseMode;
using ::esc::Signals;

/**
 * Represents the terminal itself, providing an event loop and screen writing tools.
 */
class Terminal {
   public:
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
    void commit_changes(Frame const& screen, Cursor cursor = {});

    /** Read terminal input on the calling thread. */
    auto read(std::chrono::milliseconds timeout) -> std::optional<Event>;

    /**
     * Returns the current size of the terminal.
     *
     * @return Area The current dimensions of the terminal.
     */
    [[nodiscard]] auto size() -> Area;

   private:
    Frame current_screen_{{0, 0}};
    bool screen_valid_ = false;
    Color last_foreground_ = default_color;
    Color last_background_ = default_color;
    esc::Terminal session_;
    std::string escape_sequence_;
};

}  // namespace ox

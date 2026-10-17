/** @file
 * Escape input types and TermOx dispatch effects and custom events.
 *
 * `KeyEvent`, `Key`, `KeyCode`, `KeyAction`, `Modifiers`, and the mouse event
 * types are re-exported from Escape. A key event contains its key identity,
 * modifiers, action (press/repeat/release), and optional associated Unicode text.
 * Use `matches()` for shortcuts and input_text() for printable text. Mouse
 * positions delivered to Widget handlers are local terminal-cell coordinates.
 *
 * Paste, resize, focus, and terminal protocol events retain their `esc::` names.
 * See [Escape's event declarations](https://github.com/a-n-t-h-o-n-y/Escape/blob/c714abfa0061526ec0d9ab593b4a8ab447c1d412/include/esc/event.hpp).
 * @see Event, EventResponse, Application::dispatch()
 */
#pragma once

#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <variant>

#include <esc/event.hpp>
#include <esc/key.hpp>
#include <esc/mouse.hpp>
#include <esc/text.hpp>

namespace ox {

using ::esc::Key;
using ::esc::KeyAction;
using ::esc::KeyCode;
using ::esc::KeyEvent;
using ::esc::Modifiers;
using ::esc::MouseButton;
using ::esc::MouseMove;
using ::esc::MousePress;
using ::esc::MouseRelease;
using ::esc::MouseWheel;

/**
 * Return text from a key event, excluding releases and control shortcuts.
 * Associated text can contain multiple scalars; ordinary keys carry a scalar identity.
 */
[[nodiscard]] inline auto input_text(KeyEvent const& event) -> std::u32string
{
    if (event.action == KeyAction::Release || event.modifiers.ctrl ||
        event.modifiers.alt || event.modifiers.meta || event.modifiers.super ||
        event.modifiers.hyper) {
        return {};
    }
    auto text = event.text;
    if (text.empty() && event.key) {
        if (auto scalar = esc::text_scalar(*event.key)) { text.push_back(*scalar); }
    }
    std::erase_if(text, [](char32_t scalar) {
        return scalar < U' ' || (scalar >= U'\x7f' && scalar <= U'\x9f');
    });
    return text;
}

/** Handling, painting, and exiting are independent decisions. */
struct EventResponse {
    /** Stop ancestor input propagation when true; this alone does not request repaint. */
    bool handled = false;
    /** Request a coalesced frame after dispatch; ignored once quitting takes precedence. */
    bool repaint = false;
    /** Optional exit status; combine() preserves the first requested status. */
    std::optional<int> quit = std::nullopt;
};

/** Combine effects; the first exit request wins. */
[[nodiscard]] inline auto combine(EventResponse a, EventResponse b) -> EventResponse
{
    return {.handled = a.handled || b.handled,
            .repaint = a.repaint || b.repaint,
            .quit = a.quit ? a.quit : b.quit};
}

// Events
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace event {

/** Posted application-thread work represented as an event. */
struct Custom {
    /** Callback invoked at dispatch; captures must remain valid until delivery or event
     * destruction.
     */
    std::function<EventResponse()> action;
};

/** Termination notification mapped to an exit request by dispatch(). */
struct Interrupt {};

}  // namespace event

/** Owned input, resize, custom-work, and interrupt payload accepted by EventLoop::post(). */
using Event = std::variant<esc::MousePress,
                           esc::MouseRelease,
                           esc::MouseWheel,
                           esc::MouseMove,
                           esc::KeyEvent,
                           esc::Focus,
                           esc::PasteChunk,
                           esc::UnknownInput,
                           esc::TerminalReply,
                           esc::Resize,
                           event::Custom,
                           event::Interrupt>;

}  // namespace ox

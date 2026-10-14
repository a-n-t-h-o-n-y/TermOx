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

#include <ox/core/common.hpp>

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
    bool handled = false;
    bool repaint = false;
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

struct Custom {
    std::function<EventResponse()> action;
};

struct Interrupt {};

}  // namespace event

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

#pragma once

#include <concepts>
#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
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

/** Request an application exit with the given return code. */
struct QuitRequest {
    int return_code = 0;
};

/** Handling, painting, and exiting are independent decisions. */
struct EventResponse {
    bool handled = false;
    bool repaint = false;
    std::optional<int> quit = std::nullopt;
};

// Events
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace event {

struct Timer {
    int id;
};

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
                           event::Timer,
                           event::Custom,
                           event::Interrupt>;

// Event Handler Concepts
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Checks if a type can handle a MousePress event.
 */
template <typename T>
concept HandlesMousePress = requires(T t, MousePress m) {
    { t.handle_mouse_press(m) } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a MouseRelease event.
 */
template <typename T>
concept HandlesMouseRelease = requires(T t, MouseRelease m) {
    { t.handle_mouse_release(m) } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a MouseWheel event.
 */
template <typename T>
concept HandlesMouseWheel = requires(T t, MouseWheel m) {
    { t.handle_mouse_wheel(m) } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a MouseMove event.
 */
template <typename T>
concept HandlesMouseMove = requires(T t, MouseMove m) {
    { t.handle_mouse_move(m) } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a key press or repeat.
 */
template <typename T>
concept HandlesKeyPress = requires(T t, KeyEvent k) {
    { t.handle_key_press(k) } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a key release.
 */
template <typename T>
concept HandlesKeyRelease = requires(T t, KeyEvent k) {
    { t.handle_key_release(k) } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a Resize event.
 */
template <typename T>
concept HandlesResize = requires(T t, esc::Area a) {
    { t.handle_resize(a) } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a Timer event.
 */
template <typename T>
concept HandlesTimer = requires(T t, int id) {
    { t.handle_timer(id) } -> std::same_as<EventResponse>;
};

}  // namespace ox
#include <termox/terminal/input.hpp>

#include <cstddef>
#include <optional>

#ifndef _XOPEN_SOURCE_EXTENDED
#    define _XOPEN_SOURCE_EXTENDED
#endif
#include <ncursesw/ncurses.h>
#undef border

#include <termox/system/detail/find_widget_at.hpp>
#include <termox/system/detail/focus.hpp>
#include <termox/system/event.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/system/shortcuts.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

namespace {
using namespace ox;

/// Check if mouse_event is a button_mask type of event.
template <typename Mask_t>
auto is(Mask_t button_mask, ::MEVENT const& mouse_event) -> bool
{
    return static_cast<bool>(mouse_event.bstate & button_mask);
}

auto extract_modifiers(::MEVENT const& mouse_event) -> Mouse::Modifiers
{
    return {is(BUTTON_SHIFT, mouse_event), is(BUTTON_CTRL, mouse_event),
            is(BUTTON_ALT, mouse_event)};
}

auto extract_button(::MEVENT const& state) -> std::optional<Mouse::Button>
{
    if (is(BUTTON1_PRESSED, state))
        return Mouse::Button::Left;
    else if (is(BUTTON1_RELEASED, state))
        return Mouse::Button::Left;
    else if (is(BUTTON1_DOUBLE_CLICKED, state))
        return Mouse::Button::Left;

    else if (is(BUTTON2_PRESSED, state))
        return Mouse::Button::Middle;
    else if (is(BUTTON2_RELEASED, state))
        return Mouse::Button::Middle;
    else if (is(BUTTON2_DOUBLE_CLICKED, state))
        return Mouse::Button::Middle;

    else if (is(BUTTON3_PRESSED, state))
        return Mouse::Button::Right;
    else if (is(BUTTON3_RELEASED, state))
        return Mouse::Button::Right;
    else if (is(BUTTON3_DOUBLE_CLICKED, state))
        return Mouse::Button::Right;

    else if (is(BUTTON4_PRESSED, state))
        return Mouse::Button::ScrollUp;
    else if (is(BUTTON4_RELEASED, state))
        return Mouse::Button::ScrollUp;

#if defined(BUTTON5_PRESSED) && defined(BUTTON5_RELEASED)
    else if (is(BUTTON5_PRESSED, state))
        return Mouse::Button::ScrollDown;
    else if (is(BUTTON5_RELEASED, state))
        return Mouse::Button::ScrollDown;
#endif

    return std::nullopt;
}

auto make_event(Widget& receiver, Mouse const& mouse, ::MEVENT const& state)
    -> std::optional<Event>
{
    if (is(BUTTON1_PRESSED, state))
        return Mouse_press_event{receiver, mouse};
    else if (is(BUTTON2_PRESSED, state))
        return Mouse_press_event{receiver, mouse};
    else if (is(BUTTON3_PRESSED, state))
        return Mouse_press_event{receiver, mouse};

    else if (is(BUTTON1_RELEASED, state))
        return Mouse_release_event{receiver, mouse};
    else if (is(BUTTON2_RELEASED, state))
        return Mouse_release_event{receiver, mouse};
    else if (is(BUTTON3_RELEASED, state))
        return Mouse_release_event{receiver, mouse};
    else if (is(BUTTON4_RELEASED, state))
        return Mouse_release_event{receiver, mouse};
#if defined(BUTTON5_RELEASED)
    else if (is(BUTTON5_RELEASED, state))
        return Mouse_release_event{receiver, mouse};
#endif

    else if (is(BUTTON1_DOUBLE_CLICKED, state))
        return Mouse_double_click_event{receiver, mouse};
    else if (is(BUTTON2_DOUBLE_CLICKED, state))
        return Mouse_double_click_event{receiver, mouse};
    else if (is(BUTTON3_DOUBLE_CLICKED, state))
        return Mouse_double_click_event{receiver, mouse};

    else if (is(BUTTON4_PRESSED, state))
        return Mouse_wheel_event{receiver, mouse};
#if defined(BUTTON5_PRESSED)
    else if (is(BUTTON5_PRESSED, state))
        return Mouse_wheel_event{receiver, mouse};
#endif

    return std::nullopt;
}

auto make_mouse_event() -> std::optional<Event>
{
    auto mouse_event = ::MEVENT{};
    if (::getmouse(&mouse_event) != OK)
        return std::nullopt;

    auto const global = Point{static_cast<std::size_t>(mouse_event.x),
                              static_cast<std::size_t>(mouse_event.y)};

    Widget* receiver = detail::find_widget_at(global);
    if (receiver == nullptr)
        return std::nullopt;

    auto const local =
        Point{global.x - receiver->inner_x(), global.y - receiver->inner_y()};

    auto const modifiers = extract_modifiers(mouse_event);
    auto const button    = extract_button(mouse_event);
    if (!button.has_value())
        return std::nullopt;
    auto const mouse = Mouse{global, local, *button, mouse_event.id, modifiers};
    return make_event(*receiver, mouse, mouse_event);
}

auto make_resize_event() -> std::optional<Event>
{
    if (Widget* const receiver = System::head(); receiver != nullptr)
        return Resize_event{*receiver, System::terminal.area()};
    else
        return std::nullopt;
}

auto make_keyboard_event(int input) -> std::optional<Event>
{
    auto const key = static_cast<Key>(input);
    // TODO shortcut handling is stuffed in here, should be elsewhere.
    if (Shortcuts::send_key(key))
        return std::nullopt;
    else if (auto* const r = detail::Focus::focus_widget(); r != nullptr)
        return Key_press_event{*r, key};
    return std::nullopt;
}

}  // namespace

namespace ox::input {

auto get() -> std::optional<Event>
{
    auto const input = ::getch();
    switch (input) {
        case ERR: return std::nullopt;  // Timeout and no event.
        case KEY_MOUSE: return make_mouse_event();
        case KEY_RESIZE: return make_resize_event();
        default: return make_keyboard_event(input);
    }
}

}  // namespace ox::input

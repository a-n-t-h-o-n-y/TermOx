#include <cppurses/terminal/input.hpp>

#include <cstddef>
#include <memory>
#include <tuple>
#include <utility>

#include <ncursesw/ncurses.h>
#undef border

#include <cppurses/system/detail/find_widget_at.hpp>
#include <cppurses/system/detail/focus.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/events/key.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/system/events/resize_event.hpp>
#include <cppurses/system/shortcuts.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;

/// Check if mouse_event is a button_mask type of event.
template <typename Mask_t>
auto is(Mask_t button_mask, ::MEVENT const& mouse_event) -> bool
{
    return static_cast<bool>(mouse_event.bstate & button_mask);
}

/// Extract the Event type and Mouse::Button from a given MEVENT object.
auto extract_info(::MEVENT const& mouse_event)
    -> std::tuple<Event::Type, Mouse::Button, Mouse::State::Modifiers>
{
    auto type_button_mod = std::make_tuple(Event::None, Mouse::Button::None,
                                           Mouse::State::Modifiers{});
    auto& type           = std::get<0>(type_button_mod);
    auto& button         = std::get<1>(type_button_mod);
    auto& mod            = std::get<2>(type_button_mod);
    // Button 1 / Left Button
    if (is(BUTTON1_PRESSED, mouse_event)) {
        type   = Event::MouseButtonPress;
        button = Mouse::Button::Left;
    }
    else if (is(BUTTON1_RELEASED, mouse_event)) {
        type   = Event::MouseButtonRelease;
        button = Mouse::Button::Left;
    }
    else if (is(BUTTON1_DOUBLE_CLICKED, mouse_event)) {
        type   = Event::MouseButtonDblClick;
        button = Mouse::Button::Left;
    }
    // Button 2 / Middle Button
    else if (is(BUTTON2_PRESSED, mouse_event)) {
        type   = Event::MouseButtonPress;
        button = Mouse::Button::Middle;
    }
    else if (is(BUTTON2_RELEASED, mouse_event)) {
        type   = Event::MouseButtonRelease;
        button = Mouse::Button::Middle;
    }
    else if (is(BUTTON2_DOUBLE_CLICKED, mouse_event)) {
        type   = Event::MouseButtonDblClick;
        button = Mouse::Button::Middle;
    }
    // Button 3 / Right Button
    else if (is(BUTTON3_PRESSED, mouse_event)) {
        type   = Event::MouseButtonPress;
        button = Mouse::Button::Right;
    }
    else if (is(BUTTON3_RELEASED, mouse_event)) {
        type   = Event::MouseButtonRelease;
        button = Mouse::Button::Right;
    }
    else if (is(BUTTON3_DOUBLE_CLICKED, mouse_event)) {
        type   = Event::MouseButtonDblClick;
        button = Mouse::Button::Right;
    }
    // Button 4 / Scroll Up
    else if (is(BUTTON4_PRESSED, mouse_event)) {
        type   = Event::MouseButtonPress;
        button = Mouse::Button::ScrollUp;
    }
    else if (is(BUTTON4_RELEASED, mouse_event)) {
        type   = Event::MouseButtonRelease;
        button = Mouse::Button::ScrollUp;
    }
    // Button 5 / Scroll Down
#if defined(BUTTON5_PRESSED) and defined(BUTTON5_RELEASED)
    else if (is(BUTTON5_PRESSED, mouse_event)) {
        type   = Event::MouseButtonPress;
        button = Mouse::Button::ScrollDown;
    }
    else if (is(BUTTON5_RELEASED, mouse_event)) {
        type   = Event::MouseButtonRelease;
        button = Mouse::Button::ScrollDown;
    }
#endif
    // Modifier Keys
    mod.shift = is(BUTTON_SHIFT, mouse_event);
    mod.ctrl  = is(BUTTON_CTRL, mouse_event);
    mod.alt   = is(BUTTON_ALT, mouse_event);
    return type_button_mod;
}

auto make_mouse_event() -> std::unique_ptr<Event>
{
    auto mouse_event = ::MEVENT{};
    if (::getmouse(&mouse_event) != OK)
        return nullptr;
    Widget* receiver = detail::find_widget_at(mouse_event.x, mouse_event.y);
    if (receiver == nullptr)
        return nullptr;

    // Coordinates
    auto const global = Point{static_cast<std::size_t>(mouse_event.x),
                              static_cast<std::size_t>(mouse_event.y)};
    auto const local =
        Point{global.x - receiver->inner_x(), global.y - receiver->inner_y()};

    // Create Event
    auto const [type, button, modifiers] = extract_info(mouse_event);
    if (type == Event::MouseButtonPress) {
        switch (button) {
            case Mouse::Button::ScrollUp:
            case Mouse::Button::ScrollDown:
                return std::make_unique<Mouse::Wheel>(
                    *receiver, Mouse::State{button, modifiers, global, local,
                                            mouse_event.id});
                break;
            case Mouse::Button::Left:
            case Mouse::Button::Middle:
            case Mouse::Button::Right:
                return std::make_unique<Mouse::Press>(
                    *receiver, Mouse::State{button, modifiers, global, local,
                                            mouse_event.id});
                break;
            default: break;
        }
    }
    else if (type == Event::MouseButtonRelease) {
        return std::make_unique<Mouse::Release>(
            *receiver,
            Mouse::State{button, modifiers, global, local, mouse_event.id});
    }
    else if (type == Event::MouseButtonDblClick) {
        return std::make_unique<Mouse::Double_click>(
            *receiver,
            Mouse::State{button, modifiers, global, local, mouse_event.id});
    }
    return nullptr;
}

auto make_resize_event() -> std::unique_ptr<Event>
{
    auto* const receiver = System::head();
    if (receiver != nullptr) {
        auto const width  = System::terminal.width();
        auto const height = System::terminal.height();
        return std::make_unique<Resize_event>(*receiver, Area{width, height});
    }
    return nullptr;
}

auto make_keyboard_event(int input) -> std::unique_ptr<Event>
{
    auto const code = static_cast<Key::Code>(input);
    auto* const receiver =
        Shortcuts::send_key(code) ? nullptr : detail::Focus::focus_widget();
    return receiver == nullptr ? nullptr
                               : std::make_unique<Key::Press>(*receiver, code);
}

}  // namespace

namespace cppurses {
namespace input {

auto get() -> std::unique_ptr<Event>
{
    auto const input = ::getch();
    switch (input) {
        case ERR: return nullptr;  // Timeout and no event.
        case KEY_MOUSE: return make_mouse_event();
        case KEY_RESIZE: return make_resize_event();
        default: return make_keyboard_event(input);  // Key_event
    }
}

}  // namespace input
}  // namespace cppurses

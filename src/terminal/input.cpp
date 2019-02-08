#include <cppurses/terminal/input.hpp>

#include <cstddef>
#include <memory>
#include <utility>

#include <ncurses.h>

#include <cppurses/system/detail/find_widget_at.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/events/key_event.hpp>
#include <cppurses/system/events/mouse_event.hpp>
#include <cppurses/system/events/resize_event.hpp>
#include <cppurses/system/events/terminal_resize_event.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/system/mouse_data.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;
// #undef border  // NCurses macro, naming conflict. REMOVE THIS

bool resize_happened{false};

/// If a resize actually occured, return event with the head Widget as receiver.
std::unique_ptr<Event> make_terminal_resize_event() {
    if (resize_happened) {
        resize_happened = false;
        Widget* const receiver = System::head();
        if (receiver != nullptr) {
            return std::make_unique<Terminal_resize_event>(*receiver);
        }
    }
    return nullptr;
}

/// Check if mouse_event is a button_mask type of event.
template <typename Mask_t>
bool is(Mask_t button_mask, const ::MEVENT& mouse_event) {
    return static_cast<bool>(mouse_event.bstate & button_mask);
}

/// Extract the Event type and Mouse_button from a given MEVENT object.
std::pair<Event::Type, Mouse_button> extract_info(const ::MEVENT& mouse_event) {
    auto type_button = std::make_pair(Event::None, Mouse_button::None);
    auto& type = type_button.first;
    auto& button = type_button.second;
    // Button 1 / Left Button
    if (is(BUTTON1_PRESSED, mouse_event)) {
        type = Event::MouseButtonPress;
        button = Mouse_button::Left;
    } else if (is(BUTTON1_RELEASED, mouse_event)) {
        type = Event::MouseButtonRelease;
        button = Mouse_button::Left;
    }
    // Button 2 / Middle Button
    else if (is(BUTTON2_PRESSED, mouse_event)) {
        type = Event::MouseButtonPress;
        button = Mouse_button::Middle;
    } else if (is(BUTTON2_RELEASED, mouse_event)) {
        type = Event::MouseButtonRelease;
        button = Mouse_button::Middle;
    }
    // Button 3 / Right Button
    else if (is(BUTTON3_PRESSED, mouse_event)) {
        type = Event::MouseButtonPress;
        button = Mouse_button::Right;
    } else if (is(BUTTON3_RELEASED, mouse_event)) {
        type = Event::MouseButtonRelease;
        button = Mouse_button::Right;
    }
    // Button 4 / Scroll Up
    else if (is(BUTTON4_PRESSED, mouse_event)) {
        type = Event::MouseButtonPress;
        button = Mouse_button::ScrollUp;
    } else if (is(BUTTON4_RELEASED, mouse_event)) {
        type = Event::MouseButtonRelease;
        button = Mouse_button::ScrollUp;
    }
    // Button 5 / Scroll Down
#if defined(BUTTON5_PRESSED) && defined(BUTTON5_RELEASED)
    else if (is(BUTTON5_PRESSED, mouse_event)) {
        type = Event::MouseButtonPress;
        button = Mouse_button::ScrollDown;
    } else if (is(BUTTON5_RELEASED, mouse_event)) {
        type = Event::MouseButtonRelease;
        button = Mouse_button::ScrollDown;
    }
#endif
    return type_button;
}

std::unique_ptr<Event> make_mouse_event() {
    auto mouse_event = ::MEVENT{};
    if (::getmouse(&mouse_event) != OK) {
        return nullptr;
    }
    Widget* receiver = detail::find_widget_at(mouse_event.x, mouse_event.y);
    if (receiver == nullptr) {
        return nullptr;
    }

    // Coordinates
    const auto global = Point{static_cast<std::size_t>(mouse_event.x),
                              static_cast<std::size_t>(mouse_event.y)};
    const auto local =
        Point{global.x - receiver->inner_x(), global.y - receiver->inner_y()};

    // Create Event
    const auto type_button = extract_info(mouse_event);
    const auto type = type_button.first;
    const auto button = type_button.second;
    auto event = std::unique_ptr<Event>{nullptr};
    if (type == Event::MouseButtonPress) {
        event = std::make_unique<Mouse_press_event>(
            *receiver, Mouse_data{button, global, local, mouse_event.id});
    } else if (type == Event::MouseButtonRelease) {
        event = std::make_unique<Mouse_release_event>(
            *receiver, Mouse_data{button, global, local, mouse_event.id});
    }
    return event;
}

std::unique_ptr<Event> make_resize_event() {
    Widget* const receiver = System::head();
    if (receiver != nullptr) {
        const auto width = System::terminal.width();
        const auto height = System::terminal.height();
        return std::make_unique<Resize_event>(*receiver, Area{width, height});
    }
    return nullptr;
}

std::unique_ptr<Event> make_keyboard_event(int input) {
    Widget* const receiver = Focus::focus_widget();
    return receiver != nullptr ? std::make_unique<Key_press_event>(
                                     *receiver, static_cast<Key>(input))
                               : nullptr;
}
}  // namespace

namespace cppurses {
namespace input {

// TODO
// getch() calls wrefresh() internally, making this not thread safe with
// multiple event loops running. Either have to find a simpler function or
// handle input manually w/o ncurses.
std::unique_ptr<Event> get() {
    const auto input = int{::getch()};
    auto event = std::unique_ptr<Event>{nullptr};
    switch (input) {
        case ERR:  // Terminal Screen Resize
            event = make_terminal_resize_event();
            break;
        case KEY_MOUSE:
            event = make_mouse_event();
            break;
        case KEY_RESIZE:
            event = make_resize_event();
            break;
        default:  // Key_event
            event = make_keyboard_event(input);
            break;
    }
    return event;
}

void indicate_resize(int) {
    resize_happened = true;
}

}  // namespace input
}  // namespace cppurses

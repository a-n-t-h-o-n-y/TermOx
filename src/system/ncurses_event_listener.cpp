#include "system/detail/ncurses_event_listener.hpp"
#include "system/event.hpp"
#include "system/events/key_event.hpp"
#include "system/events/mouse_event.hpp"
#include "system/events/resize_event.hpp"
#include "system/key.hpp"
#include "system/system.hpp"
#include "widget/widget.hpp"

#include <ncurses.h>

#include <cstddef>
#include <memory>

namespace {

cppurses::Widget* find_widget_at(std::size_t x, std::size_t y) {
    cppurses::Widget* widg = cppurses::System::head();
    if (widg == nullptr || !widg->has_coordinates(x, y)) {
        return nullptr;
    }
    bool keep_going = true;
    while (keep_going && !widg->children().empty()) {
        for (cppurses::Widget* child : widg->children()) {
            if (child->has_coordinates(x, y) && child->enabled()) {
                widg = child;
                keep_going = true;
                break;
            }
            keep_going = false;
        }
    }
    return widg;
}

}  // namespace

namespace cppurses {
namespace detail {

std::unique_ptr<Event> NCurses_event_listener::get_input() const {
    int input = ::getch();  // blocking call
    std::unique_ptr<Event> event{nullptr};
    switch (input) {
        case KEY_MOUSE:
            event = parse_mouse_event();
            break;

        case KEY_RESIZE:
            event = handle_resize_event();
            event->set_receiver(handle_resize_widget());
            break;

        default:  // Key_event
            event = handle_keyboard_event(input);
            event->set_receiver(handle_keyboard_widget());
            break;
    }
    return event;
}

#undef border  // NCurses macro, naming conflict.

std::unique_ptr<Event> NCurses_event_listener::parse_mouse_event() const {
    ::MEVENT mouse_event;  // NOLINT
    if (::getmouse(&mouse_event) != OK) {
        return nullptr;
    }

    Widget* receiver = find_widget_at(mouse_event.x, mouse_event.y);
    if (receiver == nullptr) {
        return nullptr;
    }

    // Parse NCurses Event
    Event::Type type = Event::None;
    Mouse_button button = Mouse_button::None;
    // Button 1 / Left Button
    if (static_cast<bool>(mouse_event.bstate & BUTTON1_PRESSED)) {
        type = Event::MouseButtonPress;
        button = Mouse_button::Left;
    } else if (static_cast<bool>(mouse_event.bstate & BUTTON1_RELEASED)) {
        type = Event::MouseButtonRelease;
        button = Mouse_button::Left;
    }
    // Button 2 / Middle Button
    else if (static_cast<bool>(mouse_event.bstate & BUTTON2_PRESSED)) {
        type = Event::MouseButtonPress;
        button = Mouse_button::Middle;
    } else if (static_cast<bool>(mouse_event.bstate & BUTTON2_RELEASED)) {
        type = Event::MouseButtonRelease;
        button = Mouse_button::Middle;
    }
    // Button 3 / Right Button
    else if (static_cast<bool>(mouse_event.bstate & BUTTON3_PRESSED)) {
        type = Event::MouseButtonPress;
        button = Mouse_button::Right;
    } else if (static_cast<bool>(mouse_event.bstate & BUTTON3_RELEASED)) {
        type = Event::MouseButtonRelease;
        button = Mouse_button::Right;
    }
    // Button 4 / Scroll Up
    else if (static_cast<bool>(mouse_event.bstate & BUTTON4_PRESSED)) {
        type = Event::MouseButtonPress;
        button = Mouse_button::ScrollUp;
    } else if (static_cast<bool>(mouse_event.bstate & BUTTON4_RELEASED)) {
        type = Event::MouseButtonRelease;
        button = Mouse_button::ScrollUp;
    }
    // Button 5 / Scroll Down
    else if (static_cast<bool>(mouse_event.bstate & BUTTON5_PRESSED)) {
        type = Event::MouseButtonPress;
        button = Mouse_button::ScrollDown;
    } else if (static_cast<bool>(mouse_event.bstate & BUTTON5_RELEASED)) {
        type = Event::MouseButtonRelease;
        button = Mouse_button::ScrollDown;
    } else {
        return nullptr;
    }

    // Location
    std::size_t local_x =
        mouse_event.x - receiver->x() - west_border_offset(receiver->border());
    std::size_t local_y =
        mouse_event.y - receiver->y() - north_border_offset(receiver->border());

    // Create Event
    std::unique_ptr<Event> event{nullptr};
    if (type == Event::MouseButtonPress) {
        event = std::make_unique<Mouse_press_event>(
            receiver, button, mouse_event.x, mouse_event.y, local_x, local_y,
            mouse_event.id);
    } else if (type == Event::MouseButtonRelease) {
        event = std::make_unique<Mouse_release_event>(
            receiver, button, mouse_event.x, mouse_event.y, local_x, local_y,
            mouse_event.id);
    } else {
        return nullptr;
    }
    return event;
}

std::unique_ptr<Event> NCurses_event_listener::handle_keyboard_event(
    int input) const {
    return std::make_unique<Key_press_event>(nullptr, static_cast<Key>(input));
}

Widget* NCurses_event_listener::handle_keyboard_widget() const {
    return System::focus_widget();
}

std::unique_ptr<Event> NCurses_event_listener::handle_resize_event() const {
    return std::make_unique<Resize_event>(nullptr, System::max_width(),
                                          System::max_height());
}

Widget* NCurses_event_listener::handle_resize_widget() const {
    return System::head();
}

}  // namespace detail
}  // namespace cppurses

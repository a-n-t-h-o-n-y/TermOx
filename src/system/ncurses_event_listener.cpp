#include <cppurses/system/detail/ncurses_event_listener.hpp>

#include <cstddef>
#include <memory>
#include <mutex>

#include <cppurses/painter/detail/ncurses_data.hpp>
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

#include <ncurses.h>

namespace cppurses {
namespace detail {

std::unique_ptr<Event> NCurses_event_listener::get_input() const {
    // TODO
    // getch() calls wrefresh() internally, making this not thread safe with
    // multiple event loops running. Either have to find a simpler function or
    // handler input manually. Cannot use a mutex here, otherwise you have to
    // wait for input before you can refresh the screen from anywhere else.
    const auto input = ::getch();
    auto event = std::unique_ptr<Event>{nullptr};
    Widget* receiver = nullptr;
    switch (input) {
        case ERR:
            // Terminal Screen Resize
            if (NCurses_data::resize_happened) {
                NCurses_data::resize_happened = false;
                if (System::head() != nullptr) {
                    event = std::make_unique<Terminal_resize_event>(
                        *System::head());
                }
            }
            break;
        case KEY_MOUSE:
            event = parse_mouse_event();
            break;
        case KEY_RESIZE:
            receiver = handle_resize_widget();
            if (receiver != nullptr) {
                event = handle_resize_event(*receiver);
            }
            break;
        default:  // Key_event
            receiver = handle_keyboard_widget();
            if (receiver != nullptr) {
                event = handle_keyboard_event(*receiver, input);
            }
            break;
    }
    return event;
}

#undef border  // NCurses macro, naming conflict.

std::unique_ptr<Event> NCurses_event_listener::parse_mouse_event() const {
    auto mouse_event = ::MEVENT{};
    if (::getmouse(&mouse_event) != OK) {
        return nullptr;
    }
    auto* receiver = detail::find_widget_at(mouse_event.x, mouse_event.y);
    if (receiver == nullptr) {
        return nullptr;
    }
    // Parse NCurses Event
    auto type = Event::None;
    auto button = Mouse_button::None;
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
#if defined(BUTTON5_PRESSED) && defined(BUTTON5_RELEASED)
    else if (static_cast<bool>(mouse_event.bstate & BUTTON5_PRESSED)) {
        type = Event::MouseButtonPress;
        button = Mouse_button::ScrollDown;
    } else if (static_cast<bool>(mouse_event.bstate & BUTTON5_RELEASED)) {
        type = Event::MouseButtonRelease;
        button = Mouse_button::ScrollDown;
    }
#endif
    else {
        return nullptr;
    }

    // Location
    const auto mouse_x = static_cast<std::size_t>(mouse_event.x);
    const auto mouse_y = static_cast<std::size_t>(mouse_event.y);
    const auto local_x = mouse_x - receiver->inner_x();
    const auto local_y = mouse_y - receiver->inner_y();

    // Create Event
    auto event = std::unique_ptr<Event>{nullptr};
    if (type == Event::MouseButtonPress) {
        event = std::make_unique<Mouse_press_event>(
            *receiver, Mouse_data{button, Point{mouse_x, mouse_y},
                                  Point{local_x, local_y}, mouse_event.id});
    } else if (type == Event::MouseButtonRelease) {
        event = std::make_unique<Mouse_release_event>(
            *receiver, Mouse_data{button, Point{mouse_x, mouse_y},
                                  Point{local_x, local_y}, mouse_event.id});
    } else {
        return nullptr;
    }
    return event;
}

std::unique_ptr<Event> NCurses_event_listener::handle_keyboard_event(
    Widget& receiver,
    int input) const {
    return std::make_unique<Key_press_event>(receiver, static_cast<Key>(input));
}

Widget* NCurses_event_listener::handle_keyboard_widget() const {
    return Focus::focus_widget();
}

std::unique_ptr<Event> NCurses_event_listener::handle_resize_event(
    Widget& receiver) const {
    // TODO the below call is not thread safe. Should be updated when the event
    // invoker is called for the main thread.
    System::terminal.update_dimensions();
    const auto width = System::terminal.width();
    const auto height = System::terminal.height();
    return std::make_unique<Resize_event>(receiver, Area{width, height});
}

Widget* NCurses_event_listener::handle_resize_widget() const {
    return System::head();
}

}  // namespace detail
}  // namespace cppurses

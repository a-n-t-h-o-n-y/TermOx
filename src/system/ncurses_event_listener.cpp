#include <cppurses/system/detail/ncurses_event_listener.hpp>

#include <cstddef>
#include <memory>
#include <vector>

#include <stdio.h>
#include <sys/ioctl.h>

#include <cppurses/painter/detail/ncurses_data.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/events/key_event.hpp>
#include <cppurses/system/events/mouse_event.hpp>
#include <cppurses/system/events/resize_event.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

#include <ncurses.h>

// #include <utility/log.hpp>

namespace {

cppurses::Widget* find_widget_at(std::size_t x, std::size_t y) {
    cppurses::Widget* widg = cppurses::System::head();
    if (widg == nullptr || !has_coordinates(*widg, x, y)) {
        return nullptr;
    }
    bool keep_going = true;
    while (keep_going && !widg->children().empty()) {
        for (cppurses::Widget* child : widg->children()) {
            if (has_coordinates(*child, x, y) && child->enabled()) {
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
    // utility::Log l;

    int input = ::getch();  // blocking call
    // l << "Input Detected : ";
    std::unique_ptr<Event> event{nullptr};
    switch (input) {
        case KEY_MOUSE:
            // l << "Mouse Input";
            event = parse_mouse_event();
            break;

        case KEY_RESIZE:
            // l << "KEY_RESIZE";
            // throw(5);
            event = handle_resize_event();
            event->set_receiver(handle_resize_widget());
            break;

        case ERR:
            // l << "ERR - ";
            // if (NCurses_data::resize_happened) {
            //     l << "resize_happened";
            // } else {
            //     l << "SOME OTHER ERROR";
            // }

            // static bool second_err{false};
            if (NCurses_data::resize_happened) {
                NCurses_data::resize_happened = false;
                // second_err = true;
                // } else {
                // bit of hack since incorrect resize coords are sometimes sent,
                // but after a second ERR, the coords should be correct.
                // if (second_err) {
                // second_err = false;
                struct winsize w;
                ioctl(fileno(stdin), TIOCGWINSZ, &w);
                // if (ret == -1) {
                // l << "ioctl failed ";
                // } else {
                NCurses_data::ncurses_mtx.lock();
                ::resizeterm(w.ws_row, w.ws_col);
                NCurses_data::ncurses_mtx.unlock();

                // l << " rows: " << w.ws_row << " cols: " << w.ws_col;
                // }
                // event = handle_resize_event();
                // event->set_receiver(handle_resize_widget());
                // }
            }
            break;

        default:  // Key_event
            // l << "Default Input - key event";
            event = handle_keyboard_event(input);
            event->set_receiver(handle_keyboard_widget());
            break;
    }
    // l << std::endl;
    return event;
}

void NCurses_event_listener::enable_ctrl_characters() {
    ::raw();
}

void NCurses_event_listener::disable_ctrl_characters() {
    ::noraw();
    ::cbreak();
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
    std::size_t mouse_x{static_cast<std::size_t>(mouse_event.x)};
    std::size_t mouse_y{static_cast<std::size_t>(mouse_event.y)};
    std::size_t local_x = mouse_x - receiver->inner_x();
    std::size_t local_y = mouse_y - receiver->inner_y();

    // Create Event
    std::unique_ptr<Event> event{nullptr};
    if (type == Event::MouseButtonPress) {
        event = std::make_unique<Mouse_press_event>(
            receiver, button, Point{mouse_x, mouse_y}, Point{local_x, local_y},
            mouse_event.id);
    } else if (type == Event::MouseButtonRelease) {
        event = std::make_unique<Mouse_release_event>(
            receiver, button, Point{mouse_x, mouse_y}, Point{local_x, local_y},
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
    return Focus::focus_widget();
}

std::unique_ptr<Event> NCurses_event_listener::handle_resize_event() const {
    return std::make_unique<Resize_event>(
        nullptr, Area{System::max_width(), System::max_height()});
    // Tell paint_buffer that there are new cells. if the size has
    // increased in either direction. height and width separate calls,
    // sending a box along, which has a top left coord.
}

Widget* NCurses_event_listener::handle_resize_widget() const {
    return System::head();
}

}  // namespace detail
}  // namespace cppurses

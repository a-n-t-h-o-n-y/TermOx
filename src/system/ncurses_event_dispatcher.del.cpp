#include "system/detail/ncurses_event_dispatcher.hpp"
#include "system/event.hpp"
#include "system/events/key_event.hpp"
#include "system/events/mouse_event.hpp"
#include "system/events/resize_event.hpp"
#include "system/system.hpp"
#include "widget/border.hpp"
#include "widget/widget.hpp"
#include <ncurses.h>
#include <cstddef>
#include <memory>
#include <utility>

namespace cppurses {
namespace detail {

void NCurses_event_dispatcher::post_user_input() {
    std::unique_ptr<Event> event{nullptr};
    Widget* object = nullptr;

    int input = ::getch();  // blocking call

    std::pair<Widget*, std::unique_ptr<Event>> obj_event_pair;

    switch (input) {
        case KEY_MOUSE:
            obj_event_pair = parse_mouse_event();
            object = obj_event_pair.first;
            event = std::move(obj_event_pair.second);
            break;

        case KEY_RESIZE:
            event = handle_resize_event();
            object = handle_resize_object();
            break;

        default:  // Key_event
            event = handle_keyboard_event(input);
            object = handle_keyboard_object();
            break;
    }

    if (object != nullptr && event != nullptr) {
        System::post_event(object, std::move(event));
    }
}

#undef border  // NCurses macro, naming conflict.

std::pair<Widget*, std::unique_ptr<Event>>
NCurses_event_dispatcher::parse_mouse_event() {
    ::MEVENT mouse_event;  // NOLINT
    if (::getmouse(&mouse_event) == OK) {
        Widget* object = find_object(mouse_event.x, mouse_event.y);

        // Parse NCurses Event
        Event::Type ev_type = Event::None;
        Mouse_button ev_button = Mouse_button::None;

        // Button 1 / Left Button
        if (static_cast<bool>(mouse_event.bstate & BUTTON1_PRESSED)) {
            ev_type = Event::MouseButtonPress;
            ev_button = Mouse_button::Left;
        } else if (static_cast<bool>(mouse_event.bstate & BUTTON1_RELEASED)) {
            ev_type = Event::MouseButtonRelease;
            ev_button = Mouse_button::Left;
        }

        // Button 2 / Middle Button
        else if (static_cast<bool>(mouse_event.bstate & BUTTON2_PRESSED)) {
            ev_type = Event::MouseButtonPress;
            ev_button = Mouse_button::Middle;
        } else if (static_cast<bool>(mouse_event.bstate & BUTTON2_RELEASED)) {
            ev_type = Event::MouseButtonRelease;
            ev_button = Mouse_button::Middle;
        }

        // Button 3 / Right Button
        else if (static_cast<bool>(mouse_event.bstate & BUTTON3_PRESSED)) {
            ev_type = Event::MouseButtonPress;
            ev_button = Mouse_button::Right;
        } else if (static_cast<bool>(mouse_event.bstate & BUTTON3_RELEASED)) {
            ev_type = Event::MouseButtonRelease;
            ev_button = Mouse_button::Right;
        }

        // Button 4 / Scroll Up
        else if (static_cast<bool>(mouse_event.bstate & BUTTON4_PRESSED)) {
            ev_type = Event::MouseButtonPress;
            ev_button = Mouse_button::ScrollUp;
        } else if (static_cast<bool>(mouse_event.bstate & BUTTON4_RELEASED)) {
            ev_type = Event::MouseButtonRelease;
            ev_button = Mouse_button::ScrollUp;
        }

        // Button 5 / Scroll Down
        else if (static_cast<bool>(mouse_event.bstate & BUTTON5_PRESSED)) {
            ev_type = Event::MouseButtonPress;
            ev_button = Mouse_button::ScrollDown;
        } else if (static_cast<bool>(mouse_event.bstate & BUTTON5_RELEASED)) {
            ev_type = Event::MouseButtonRelease;
            ev_button = Mouse_button::ScrollDown;
        } else {
            return std::pair<Widget*, std::unique_ptr<Event>>{nullptr, nullptr};
        }

        // Determine location of mouse event.
        auto* widg = dynamic_cast<Widget*>(object);
        if (widg != nullptr) {
            auto local_x =
                mouse_event.x - widg->x() - west_border_offset(widg->border());
            auto local_y =
                mouse_event.y - widg->y() - north_border_offset(widg->border());
            auto event = std::make_unique<Mouse_event>(
                ev_type, ev_button, mouse_event.x, mouse_event.y, local_x,
                local_y, mouse_event.id);
            return std::make_pair(object, std::move(event));
        }
    }
    return std::pair<Widget*, std::unique_ptr<Event>>{nullptr, nullptr};
}

Widget* NCurses_event_dispatcher::find_object(std::size_t x, std::size_t y) {
    Widget* obj = System::head();
    if (obj == nullptr || !obj->has_coordinates(x, y)) {
        return nullptr;
    }

    bool keep_going = true;
    while (keep_going && !obj->children().empty()) {
        for (Widget* child : obj->children()) {
            if (child->has_coordinates(x, y) && child->enabled()) {
                obj = child;
                keep_going = true;
                break;
            }
            keep_going = false;
        }
    }
    return obj;
}

std::unique_ptr<Event> NCurses_event_dispatcher::handle_keyboard_event(
    int input) {
    return std::make_unique<Key_event>(Event::KeyPress, input);
}

Widget* NCurses_event_dispatcher::handle_keyboard_object() {
    return System::focus_widget();
}

std::unique_ptr<Event> NCurses_event_dispatcher::handle_resize_event() {
    return std::make_unique<Resize_event>(System::max_width(),
                                          System::max_height());
}

Widget* NCurses_event_dispatcher::handle_resize_object() {
    return System::head();
}

}  // namespace detail
}  // namespace cppurses

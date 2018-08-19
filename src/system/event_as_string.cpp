#include <cppurses/system/detail/event_as_string.hpp>

#include <string>

#include <cppurses/system/event.hpp>

namespace cppurses {
namespace detail {

std::string event_as_string(const Event& event) {
    return event_type_as_string(event.type());
}

std::string event_type_as_string(Event::Type event_type) {
    std::string name;
    switch (event_type) {
        case Event::None:
            name = "None";
            break;
        case Event::MouseButtonPress:
            name = "MouseButtonPress";
            break;
        case Event::MouseButtonRelease:
            name = "MouseButtonRelease";
            break;
        case Event::MouseButtonDblClick:
            name = "MouseButtonDblClick";
            break;
        case Event::MouseWheel:
            name = "MouseWheel";
            break;
        case Event::MouseMove:
            name = "MouseMove";
            break;
        case Event::KeyPress:
            name = "KeyPress";
            break;
        case Event::KeyRelease:
            name = "KeyRelease";
            break;
        case Event::FocusIn:
            name = "FocusIn";
            break;
        case Event::FocusOut:
            name = "FocusOut";
            break;
        case Event::Paint:
            name = "Paint";
            break;
        case Event::Move:
            name = "Move";
            break;
        case Event::Resize:
            name = "Resize";
            break;
        case Event::ChildAdded:
            name = "ChildAdded";
            break;
        case Event::ChildRemoved:
            name = "ChildRemoved";
            break;
        case Event::ChildPolished:
            name = "ChildPolished";
            break;
        case Event::Enable:
            name = "Enable";
            break;
        case Event::Disable:
            name = "Disable";
            break;
        case Event::Delete:
            name = "Delete";
            break;
        case Event::Animation:
            name = "Animation";
            break;
        default:
            name = "Other Event";
    }
    return name;
}

}  // namespace detail
}  // namespace cppurses

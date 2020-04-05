#include <cppurses/system/event.hpp>

#include <string>
#include <vector>

#include <cppurses/widget/widget.hpp>

namespace cppurses {

auto Event::send_to_all_filters() const -> bool
{
    auto const& filters = receiver_.get_event_filters();
    for (Widget* filter : filters) {
        if (filter->is_enabled() and this->filter_send(*filter))
            return true;
    }
    return false;
}

auto to_string(Event::Type type) -> std::string
{
    switch (type) {
        case Event::MouseButtonPress: return "MouseButtonPress";
        case Event::MouseButtonRelease: return "MouseButtonRelease";
        case Event::MouseButtonDblClick: return "MouseButtonDblClick";
        case Event::MouseWheel: return "MouseWheel";
        case Event::MouseMove: return "MouseMove";
        case Event::KeyPress: return "KeyPress";
        case Event::KeyRelease: return "KeyRelease";
        case Event::FocusIn: return "FocusIn";
        case Event::FocusOut: return "FocusOut";
        case Event::Paint: return "Paint";
        case Event::Move: return "Move";
        case Event::Resize: return "Resize";
        case Event::TerminalResize: return "TerminalResize";
        case Event::ChildAdded: return "ChildAdded";
        case Event::ChildRemoved: return "ChildRemoved";
        case Event::ChildPolished: return "ChildPolished";
        case Event::Enable: return "Enable";
        case Event::Disable: return "Disable";
        case Event::Delete: return "Delete";
        case Event::Timer: return "Timer";
        case Event::Custom: return "Custom";
        default: return "None";
    }
}

}  // namespace cppurses

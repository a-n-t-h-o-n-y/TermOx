#ifndef TERMOX_SYSTEM_EVENT_HPP
#define TERMOX_SYSTEM_EVENT_HPP
#include <functional>
#include <memory>
#include <variant>

#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

using Widget_ref = std::reference_wrapper<Widget>;

struct Paint_event {
    Widget_ref receiver;
};

struct Key_press_event {
    Widget_ref receiver;
    ox::Key key;
};

struct Mouse_press_event {
    Widget_ref receiver;
    Mouse data;
};

struct Mouse_release_event {
    Widget_ref receiver;
    Mouse data;
};

struct Mouse_double_click_event {
    Widget_ref receiver;
    Mouse data;
};

struct Mouse_wheel_event {
    Widget_ref receiver;
    Mouse data;
};

struct Mouse_move_event {
    Widget_ref receiver;
    Mouse data;
};

struct Child_added_event {
    Widget_ref receiver;
    Widget_ref child;
};

struct Child_removed_event {
    Widget_ref receiver;
    Widget_ref child;
};

struct Child_polished_event {
    Widget_ref receiver;
    Widget_ref child;
};

struct Delete_event {
    std::unique_ptr<Widget> removed;
};

struct Disable_event {
    Widget_ref receiver;
};

struct Enable_event {
    Widget_ref receiver;
};

struct Focus_in_event {
    Widget_ref receiver;
};

struct Focus_out_event {
    Widget_ref receiver;
};

struct Move_event {
    Widget_ref receiver;
    Point new_position;
};

struct Resize_event {
    Widget_ref receiver;
    Area new_area;
};

struct Timer_event {
    Widget_ref receiver;
};

/** \p send will be called to send the event, typically would call on a member
 *  function of some receiving Widget type. \p filter_send should call whatever
 *  filter method on each installed filter, and return true if one of the
 *  filters handled the event. */
struct Custom_event {
    std::function<void()> send;
    std::function<bool()> filter_send = [] { return false; };
};

using Event = std::variant<Paint_event,
                           Key_press_event,
                           Mouse_press_event,
                           Mouse_release_event,
                           Mouse_double_click_event,
                           Mouse_wheel_event,
                           Mouse_move_event,
                           Child_added_event,
                           Child_removed_event,
                           Child_polished_event,
                           Delete_event,
                           Disable_event,
                           Enable_event,
                           Focus_in_event,
                           Focus_out_event,
                           Move_event,
                           Resize_event,
                           Timer_event,
                           Custom_event>;

}  // namespace ox
#endif  // TERMOX_SYSTEM_EVENT_HPP

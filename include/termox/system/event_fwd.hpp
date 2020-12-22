#ifndef TERMOX_SYSTEM_EVENT_FWD_HPP
#define TERMOX_SYSTEM_EVENT_FWD_HPP
#include <variant>

namespace ox {

struct Paint_event;
struct Key_press_event;
struct Mouse_press_event;
struct Mouse_release_event;
struct Mouse_double_click_event;
struct Mouse_wheel_event;
struct Mouse_move_event;
struct Child_added_event;
struct Child_removed_event;
struct Child_polished_event;
struct Delete_event;
struct Disable_event;
struct Enable_event;
struct Focus_in_event;
struct Focus_out_event;
struct Move_event;
struct Resize_event;
struct Timer_event;
struct Custom_event;

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
#endif  // TERMOX_SYSTEM_EVENT_FWD_HPP

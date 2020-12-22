#ifndef TERMOX_SYSTEM_DETAIL_EVENT_PRINT_HPP
#define TERMOX_SYSTEM_DETAIL_EVENT_PRINT_HPP
#include <iostream>

#include <termox/system/event.hpp>

namespace ox::detail {

inline void event_print(std::ostream& os, ox::Paint_event const& e)
{
    os << "Paint_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, ox::Key_press_event const& e)
{
    os << "Key_press_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, ox::Mouse_press_event const& e)
{
    os << "Mouse_press_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, ox::Mouse_release_event const& e)
{
    os << "Mouse_release_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, ox::Mouse_double_click_event const& e)
{
    os << "Mouse_double_click_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, ox::Mouse_wheel_event const& e)
{
    os << "Mouse_wheel_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, ox::Mouse_move_event const& e)
{
    os << "Mouse_move_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, ox::Child_added_event const& e)
{
    os << "Child_added_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, ox::Child_removed_event const& e)
{
    os << "Child_removed_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, ox::Child_polished_event const& e)
{
    os << "Child_polished_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, ox::Delete_event const& e)
{
    os << "Delete_event\n";
    os << "--->receiver id:   " << e.removed->unique_id() << '\n';
    os << "--->receiver name: " << e.removed->name() << '\n';
}

inline void event_print(std::ostream& os, ox::Disable_event const& e)
{
    os << "Disable_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, ox::Enable_event const& e)
{
    os << "Enable_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, ox::Focus_in_event const& e)
{
    os << "Focus_in_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, ox::Focus_out_event const& e)
{
    os << "Focus_out_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, ox::Move_event const& e)
{
    os << "Move_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, ox::Resize_event const& e)
{
    os << "Resize_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, ox::Timer_event const& e)
{
    os << "Timer_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, ox::Custom_event const&)
{
    os << "Custom_event\n";
}

}  // namespace ox::detail
#endif  // TERMOX_SYSTEM_DETAIL_EVENT_PRINT_HPP

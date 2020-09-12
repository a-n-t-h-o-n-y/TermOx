#ifndef CPPURSES_SYSTEM_DETAIL_EVENT_PRINT_HPP
#define CPPURSES_SYSTEM_DETAIL_EVENT_PRINT_HPP
#include <iostream>

#include <cppurses/system/event.hpp>

namespace cppurses::detail {

inline void event_print(std::ostream& os, cppurses::Paint_event const& e)
{
    os << "Paint_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, cppurses::Key_press_event const& e)
{
    os << "Key_press_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, cppurses::Mouse_press_event const& e)
{
    os << "Mouse_press_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os,
                        cppurses::Mouse_release_event const& e)
{
    os << "Mouse_release_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os,
                        cppurses::Mouse_double_click_event const& e)
{
    os << "Mouse_double_click_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, cppurses::Mouse_wheel_event const& e)
{
    os << "Mouse_wheel_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, cppurses::Mouse_move_event const& e)
{
    os << "Mouse_move_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, cppurses::Child_added_event const& e)
{
    os << "Child_added_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os,
                        cppurses::Child_removed_event const& e)
{
    os << "Child_removed_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os,
                        cppurses::Child_polished_event const& e)
{
    os << "Child_polished_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, cppurses::Delete_event const& e)
{
    os << "Delete_event\n";
    os << "--->receiver id:   " << e.removed->unique_id() << '\n';
    os << "--->receiver name: " << e.removed->name() << '\n';
}

inline void event_print(std::ostream& os, cppurses::Disable_event const& e)
{
    os << "Disable_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, cppurses::Enable_event const& e)
{
    os << "Enable_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, cppurses::Focus_in_event const& e)
{
    os << "Focus_in_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, cppurses::Focus_out_event const& e)
{
    os << "Focus_out_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, cppurses::Move_event const& e)
{
    os << "Move_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, cppurses::Resize_event const& e)
{
    os << "Resize_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, cppurses::Timer_event const& e)
{
    os << "Timer_event\n";
    os << "--->receiver id:   " << e.receiver.get().unique_id() << '\n';
    os << "--->receiver name: " << e.receiver.get().name() << '\n';
}

inline void event_print(std::ostream& os, cppurses::Custom_event const&)
{
    os << "Custom_event\n";
}

}  // namespace cppurses::detail
#endif  // CPPURSES_SYSTEM_DETAIL_EVENT_PRINT_HPP

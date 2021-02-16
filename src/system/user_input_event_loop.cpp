#include <termox/system/detail/user_input_event_loop.hpp>

#include <termox/system/event.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/terminal.hpp>

namespace ox::detail {

void User_input_event_loop::loop_function()
{
    ox::System::post_event(ox::System::terminal.get());
}

}  // namespace ox::detail

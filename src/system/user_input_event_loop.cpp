#include <cppurses/system/detail/user_input_event_loop.hpp>

#include <optional>
#include <utility>

#include <cppurses/system/event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/terminal/input.hpp>

namespace cppurses::detail {

void User_input_event_loop::loop_function()
{
    if (auto event = input::get(); event != std::nullopt)
        System::post_event(std::move(*event));
}

}  // namespace cppurses::detail

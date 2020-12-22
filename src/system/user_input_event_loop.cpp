#include <termox/system/detail/user_input_event_loop.hpp>

#include <optional>
#include <utility>

#include <termox/system/event.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/input.hpp>

namespace ox::detail {

void User_input_event_loop::loop_function()
{
    if (auto event = input::get(); event != std::nullopt)
        System::post_event(std::move(*event));
}

}  // namespace ox::detail

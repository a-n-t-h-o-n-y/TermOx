#include <cppurses/system/detail/user_input_event_loop.hpp>

#include <memory>
#include <utility>

#include <cppurses/system/event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/terminal/input.hpp>

namespace cppurses {
namespace detail {

auto User_input_event_loop::loop_function() -> bool
{
    auto event = input::get();
    if (event == nullptr)
        return false;
    System::post_event(std::move(event));
    return true;
}

}  // namespace detail
}  // namespace cppurses

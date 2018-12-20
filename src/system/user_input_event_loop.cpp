#include <cppurses/system/detail/user_input_event_loop.hpp>

#include <memory>
#include <utility>

#include <cppurses/system/detail/ncurses_event_listener.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/system.hpp>

namespace cppurses {
namespace detail {

void User_input_event_loop::loop_function() {
    auto event = event_listener_.get_input();
    System::post_event(std::move(event));
}

}  // namespace detail
}  // namespace cppurses

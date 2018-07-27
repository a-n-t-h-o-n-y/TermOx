#include <cppurses/system/detail/user_input_event_loop.hpp>

#include <memory>
#include <utility>

#include <cppurses/system/detail/abstract_event_listener.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/event_loop.hpp>
#include <cppurses/system/system.hpp>

namespace {
using namespace cppurses;
void get_and_post_user_input() {
    std::unique_ptr<cppurses::Event> event{
        System::event_listener()->get_input()};
    System::post_event(std::move(event));
}
}  // namespace

namespace cppurses {
namespace detail {

User_input_event_loop::User_input_event_loop()
    : Event_loop{get_and_post_user_input} {}

}  // namespace detail
}  // namespace cppurses

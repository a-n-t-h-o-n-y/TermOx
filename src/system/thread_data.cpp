#include "system/detail/thread_data.hpp"

namespace cppurses {
namespace detail {

Thread_data& Thread_data::current() {
    static Thread_data data;
    return data;
}

Abstract_event_dispatcher& Thread_data::dispatcher() {
    return *dispatcher_;
}

}  // namespace detail
}  // namespace cppurses

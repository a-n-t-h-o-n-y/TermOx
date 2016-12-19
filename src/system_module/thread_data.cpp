#include <system_module/detail/thread_data.hpp>

namespace mcurses {
namespace detail {

Thread_data& Thread_data::current() {
    return data_;
}

Abstract_event_dispatcher& Thread_data::dispatcher() {
    return *dispatcher_;
}

Thread_data Thread_data::data_;

}  // namespace detail
}  // namespace mcurses

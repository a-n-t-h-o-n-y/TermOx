#include <system_module/detail/thread_data.hpp>

namespace mcurses {
namespace detail {

Thread_data& Thread_data::current()
{
	static Thread_data data_; // have the constructor initialize everything

	return data_;
}

Abstract_event_dispatcher& Thread_data::dispatcher()
{
	return *dispatcher_;
}

} // namespace detail
} // namespace mcurses

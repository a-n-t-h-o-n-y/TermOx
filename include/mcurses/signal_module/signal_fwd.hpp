#ifndef SIGNAL_FWD_HPP
#define SIGNAL_FWD_HPP

#include <mutex>
#include "optional_last_value.hpp"
#include <functional>
#include "detail/function_type_splitter.hpp"

namespace mcurses
{
template <typename Signature,
		  typename Combiner = mcurses::optional_last_value<typename mcurses::function_type_splitter<Signature>::return_type>,
		  typename Group = int,
		  typename GroupCompare = std::less<Group>,
		  typename SlotFunction = std::function<Signature>,
		  typename Mutex = std::mutex>
class signal;	// might need complete type
}

#endif // SIGNAL_FWD_HPP
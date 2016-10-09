#ifndef FUNCTION_TYPE_SPLITTER_HPP
#define FUNCTION_TYPE_SPLITTER_HPP

#include <tuple>

namespace mcurses
{

template <typename Function>
class function_type_splitter;

template <typename Ret, typename ... Args>
class function_type_splitter<Ret(Args...)> {
public:
	static const int arity = sizeof...(Args);

	typedef Ret return_type;
	typedef std::tuple<Args...> argument_tuple_type;
};

} // namespace mcurses

#endif // FUNCTION_TYPE_SPLITTER_HPP
/// \file
#ifndef OPTIONAL_LAST_VALUE_HPP
#define OPTIONAL_LAST_VALUE_HPP
#include "none.hpp"
#include "optional.hpp"

#include <iostream>

namespace mcurses
{

/**	\class optional_last_value
 *	\brief A function object for returning the last value from a range
 *	of iterators.
 *	
 *	The return value is wrapped in an mcurses::optional<T> object.
 *	If the iterator range is empty, an uninitialized optional object
 *	is returned. An optional can be tested as a bool to check if it
 *	is initialized, and dereferenced to access the value.
 */
template <typename T>
class optional_last_value {
public:
	//!	Needed for clients of optional_last_value.
	typedef optional<T> result_type;

	//!	Calls the function object.
	/*
		\param 	first	Input iterator to the first element in the range.
		\param 	last	Input iterator to one past the last element in the range.
		\retval			The value of the last iterator in the range dereferenced, wrapped in an optional.
						Uninitialized optional is returned if empty range is given.
	*/
	template<typename InputIterator>
	result_type operator()(InputIterator first, InputIterator last) const
	{
		if(first == last)
		{
			return optional<T>{none};
		}

		T temp{};
		while(first != last)
		{
			temp = *first;
			++first;	// this does not work because you are working on a copy, and incrementing the object address does not work.
		}
		return optional<T>(temp);
	}
};

template <>
class optional_last_value<void> {
public:
	//!	Needed for clients of optional_last_value.
	typedef void result_type;

	//!	Calls the function object
	/*
		\param 	first	Input iterator to the first element in the range.
		\param 	last	Input iterator to one past the last element in the range.
		\retval			void.
	*/
	template<typename InputIterator>
	result_type operator()(InputIterator first, InputIterator last) const
	{
		return;
	}
};

}	// namespace mcurses

#endif	// OPTIONAL_LAST_VALUE_HPP
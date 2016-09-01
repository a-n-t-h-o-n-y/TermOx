/// \file none_t.hpp
#ifndef NONE_T_HPP
#define NONE_T_HPP

namespace mcurses
{
	
/**	\struct none_helper
 *	\brief A dummy struct.
 *
 *	An implementation of the Safe Bool Idiom. Allows for objects
 *	of type none_t to implicitly evaluate to false while avoiding
 *	many of the common pitfalls with implicit bool conversions.
 */
struct none_helper {
	typedef int *none_t;
};

}	// namespace mcurses

#endif	// NONE_T_HPP
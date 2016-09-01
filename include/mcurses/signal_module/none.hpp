/// \file none.hpp
#ifndef NONE_HPP
#define NONE_HPP

#include "none_t.hpp"

/** \namespace mcurses
 *	MCurses Library wide namespace
 */
namespace mcurses
{

/**	\var none
 *	\brief Global object similar to null.
 *
 *	Used in mcurses::optional<T> as a return type to mean 'no value'.
 */
none_helper::none_t const none = static_cast<none_helper::none_t>(0);

}	// namespace mcurses

#endif	// NONE_HPP

/// \file none.hpp
#ifndef NONE_HPP
#define NONE_HPP

/** \namespace mcurses
 *	MCurses Library wide namespace
 */
namespace mcurses
{

/**	\class none_t
 *	\brief Represents no value.
 *
 *	Object of none_t type will evaluate to false.
 *	Used to initialize optional<T> to an empty/uninitialized state.
 */
class none_t {
public:
	explicit operator bool() const {return false;}
};

/**	\var none
 *	\brief Global object similar to null.
 *
 *	Used in mcurses::optional<T> as a return type to mean 'no value'.
 */
const none_t none{};

}	// namespace mcurses

#endif	// NONE_HPP

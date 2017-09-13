/// \file
/// \brief Contains definition of the None_t class and a global None_t object.
#ifndef NONE_HPP
#define NONE_HPP

/** \namespace opt
 *	Optional Library namespace
 */
namespace opt {

/// Represents 'no value', or null.
class None_t {
   public:
    /// Safe bool conversion.
    explicit operator bool() const { return false; }
} const none;

///	\var none
/// Convenience global None_t object.

}  // namespace opt

#endif  // NONE_HPP

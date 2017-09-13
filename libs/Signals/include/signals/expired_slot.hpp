/// \file
/// \brief Includes Expired_slot exception definition.
#ifndef EXPIRED_SLOT_HPP
#define EXPIRED_SLOT_HPP

#include <stdexcept>

/// \namespace
/// Signals Library namespace.
namespace sig {

/// Thrown when a Slot is accessed after it has expired.
class Expired_slot : public std::logic_error {
   public:
    /// \param what Used to identify the exception.
    explicit Expired_slot(const char* what = "") : logic_error{what} {}
};

}  // namespace sig

#endif  // EXPIRED_SLOT_HPP

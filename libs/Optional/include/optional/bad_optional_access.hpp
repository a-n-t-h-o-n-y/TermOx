/// \file
/// \brief Contains the bad_optional_access exception class definition.
#ifndef BAD_OPTIONAL_ACCESS_HPP
#define BAD_OPTIONAL_ACCESS_HPP

#include <stdexcept>

namespace opt {

/// Exception for use when an empty Optional<T> object is accessed.
class Bad_optional_access : public std::logic_error {
   public:
    /// \param what Used to identify the exception.
    explicit Bad_optional_access(const char* what) : std::logic_error(what) {}
};

}  // namespace opt

#endif  // BAD_OPTIONAL_ACCESS_HPP

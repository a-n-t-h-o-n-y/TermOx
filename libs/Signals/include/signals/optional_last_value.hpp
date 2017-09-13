/// \file
/// \brief Contains Optional_last_value template class definition and void
/// specialization.
#ifndef OPTIONAL_LAST_VALUE_HPP
#define OPTIONAL_LAST_VALUE_HPP

#include <optional/none.hpp>
#include <optional/optional.hpp>

namespace sig {

/// \brief A functor class that returns the last value in an iterator range.
///
/// Attempts to dereference every iterator in the range [first, last).
/// If the iterator range is empty, an empty opt::Optional object is returned,
/// otherwise an opt::Optional wrapping the last value is returned.
template <typename T>
class Optional_last_value {
   public:
    ///	Type of object the iterator range points to.
    using result_type = opt::Optional<T>;

    ///	\param  first  Input iterator to the first element in the range.
    ///	\param 	last   Input iterator to one past the last element in the range.
    ///	\returns	   The value stored in the last iterator of the range,
    ///                wrapped in an opt::Optional.
    template <typename InputIterator>
    result_type operator()(InputIterator first, InputIterator last) const {
        if (first == last) {
            return opt::Optional<T>{opt::none};
        }
        T temp;
        while (first != last) {
            temp = *first;
            ++first;
        }
        return opt::Optional<T>{std::move(temp)};
    }
};

/// \brief Specialization for void return type.  /
/// Useful when dereferencing of iterators produces side effects.
template <>
class Optional_last_value<void> {
   public:
    ///	Type of object the iterator range points to.
    using result_type = void;

    ///	\param  first  Input iterator to the first element in the range.
    ///	\param 	last   Input iterator to one past the last element in the range.
    template <typename InputIterator>
    result_type operator()(InputIterator first, InputIterator last) const {
        while (first != last) {
            *first;
            ++first;
        }
    }
};

}  // namespace sig

#endif  // OPTIONAL_LAST_VALUE_HPP

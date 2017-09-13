#ifndef DETAIL_SLOT_ITERATOR_HPP
#define DETAIL_SLOT_ITERATOR_HPP

#include <iterator>

namespace sig {

// Slot_iterator points to a functor object, when it is dereferenced you get a
// function type object that is callable with no arguments. The iterator is
// useful for iterating over a container of functions and calling each function
// on a dereference operation. InputIterator is any container iterator that
// holds a callable type and can be incremented, dereferenced and copied.
// \sa Signal_impl
template <typename InputIterator>
class Slot_iterator {
   public:
    using iter_result_type =
        typename std::iterator_traits<InputIterator>::value_type;
    using result_type = typename iter_result_type::result_type;

    Slot_iterator() = default;  // might never be used?

    explicit Slot_iterator(InputIterator iter) : iter_{iter} {}

    result_type operator*() {
        auto slot = *iter_;
        return slot();
    }

    Slot_iterator& operator++() {
        ++iter_;
        return *this;
    }

    bool operator==(const Slot_iterator& x) { return iter_ == x.iter_; }

    bool operator!=(const Slot_iterator& x) { return !operator==(x); }

   private:
    InputIterator iter_;
};

}  // namespace sig

#endif  // DETAIL_SLOT_ITERATOR_HPP

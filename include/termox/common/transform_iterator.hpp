#ifndef TERMOX_COMMON_TRANSFORM_ITERATOR_HPP
#define TERMOX_COMMON_TRANSFORM_ITERATOR_HPP
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

namespace ox {

/// operator*() will apply \p map to the result of the underlying deref result.
/** Should work both as a const_iterator and non-const iterator, depending on
 *  the type of iterator given at construction, and if the map_fn works in both
 *  const and non-const conditions. */
template <typename Iter, typename Map_fn>
class Transform_iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using reference  = std::invoke_result_t<Map_fn, typename Iter::reference>;
    using value_type = std::remove_reference_t<reference>;
    using pointer    = value_type*;

   public:
    Transform_iterator(Iter it, Map_fn map_fn)
        : it_{it}, map_fn_{std::move(map_fn)}
    {}

    Transform_iterator(Transform_iterator const&) = default;
    Transform_iterator(Transform_iterator&&)      = default;

    Transform_iterator& operator=(Transform_iterator const& other)
    {
        it_ = other.it_;
        return *this;
    }
    Transform_iterator& operator=(Transform_iterator&&) = default;

   public:
    auto operator++() -> Transform_iterator&
    {
        ++it_;
        return *this;
    }

    auto operator++(int) -> Transform_iterator { return {it_++, map_fn_}; }

    auto operator*() const -> reference { return map_fn_(*it_); }

    auto operator==(Transform_iterator const& other) const -> bool
    {
        return it_ == other.it_;
    }

    auto operator!=(Transform_iterator const& other) const -> bool
    {
        return it_ != other.it_;
    }

    template <typename T>
    auto operator==(T const& other) const -> bool
    {
        return it_ == other;
    }

    template <typename T>
    auto operator!=(T const& other) const -> bool
    {
        return it_ != other;
    }

    auto underlying() const -> Iter { return it_; }

   private:
    Iter it_;
    Map_fn map_fn_;
};

}  // namespace ox
#endif  // TERMOX_COMMON_TRANSFORM_ITERATOR_HPP

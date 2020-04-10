#ifndef CPPURSES_COMMON_MAP_ITERATOR_HPP
#define CPPURSES_COMMON_MAP_ITERATOR_HPP
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace cppurses {

/// Applies \p map to dereference of \p it.
template <typename Iter, typename F>
class Map_iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = std::remove_reference_t<
        std::invoke_result_t<F, typename Iter::reference>>;
    using difference_type = std::ptrdiff_t;
    using pointer         = value_type*;
    using reference       = value_type&;

   public:
    Map_iterator(Iter it, F map) : it_{it}, map_{map} {}

    auto operator++() -> Map_iterator&
    {
        ++it_;
        return *this;
    }

    auto operator++(int) -> Map_iterator { return {it_++, map_}; }

    auto operator*() const -> auto& { return map_(*it_); }

    auto operator==(Map_iterator const& other) const -> bool
    {
        return it_ == other.it_;
    }

    auto operator!=(Map_iterator const& other) const -> bool
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
    F map_;
};

}  // namespace cppurses
#endif  // CPPURSES_COMMON_MAP_ITERATOR_HPP

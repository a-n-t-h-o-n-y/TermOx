#ifndef CPPURSES_COMMON_MAP_ITERATOR_HPP
#define CPPURSES_COMMON_MAP_ITERATOR_HPP
namespace cppurses {

/// Applies \p map to dereference of \p it.
template <typename Iter, typename F>
class Map_iterator {
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

   private:
    Iter it_;
    F map_;
};

}  // namespace cppurses
#endif  // CPPURSES_COMMON_MAP_ITERATOR_HPP

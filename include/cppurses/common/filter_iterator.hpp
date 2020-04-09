#ifndef CPPURSES_COMMON_FILTER_ITERATOR_HPP
#define CPPURSES_COMMON_FILTER_ITERATOR_HPP
namespace cppurses {

/// operator++ skips underlying elements of \p it that do not satisfy predicate.
template <typename Iter, typename Iter_end, typename F>
class Filter_iterator {
   public:
    /// Pass in the iterator to begin at, end iter, and predicate to test with.
    /** This increments \p it until the end or predicate is true. */
    Filter_iterator(Iter it, Iter_end end, F predicate)
        : it_{it}, end_{end}, predicate_{predicate}
    {
        this->increment_if_invalid;
    }

    auto operator++() -> Filter_iterator&
    {
        this->increment();
        return *this;
    }

    auto operator++(int) -> Filter_iterator
    {
        auto copy = *this;
        copy.increment();
        return copy;
    }

    auto operator*() const -> auto& { return *it_; }

    auto operator==(Filter_iterator const& other) const -> bool
    {
        return it_ == other.it_;
    }

    auto operator!=(Filter_iterator const& other) const -> bool
    {
        return it_ != other.it_;
    }

   private:
    Iter it_;
    Iter_end end_;
    F predicate_;

   private:
    void increment_if_invalid()
    {
        while (it_ != end_ && !predicate_(*it_))
            ++it_;
    }

    void increment()
    {
        do {
            ++it_;
        } while (it_ != end_ && !predicate_(*it_));
    }
};

}  // namespace cppurses
#endif  // CPPURSES_COMMON_FILTER_ITERATOR_HPP

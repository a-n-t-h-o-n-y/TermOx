#ifndef TERMOX_COMMON_FILTER_ITERATOR_HPP
#define TERMOX_COMMON_FILTER_ITERATOR_HPP
#include <iterator>
#include <utility>

namespace ox {

/// operator++ skips underlying elements of \p it that do not satisfy predicate.
template <typename Iter, typename Iter_end, typename F>
class Filter_iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = typename Iter::value_type;
    using difference_type   = typename Iter::difference_type;
    using pointer           = typename Iter::pointer;
    using reference         = typename Iter::reference;

   public:
    /// Pass in the iterator to begin at, end iter, and predicate to test with.
    /** This increments \p it until the end or predicate is true. */
    Filter_iterator(Iter const it, Iter_end const end, F&& predicate)
        : it_{it}, end_{end}, predicate_{std::forward<F>(predicate)}
    {
        this->increment_if_invalid();
    }

   public:
    auto operator++() -> Filter_iterator&
    {
        this->increment();
        return *this;
    }

    [[nodiscard]] auto operator++(int) -> Filter_iterator
    {
        auto copy = *this;
        copy.increment();
        return copy;
    }

    [[nodiscard]] auto operator*() const -> auto& { return *it_; }

    [[nodiscard]] auto operator==(Filter_iterator const& other) const -> bool
    {
        return it_ == other.it_;
    }

    [[nodiscard]] auto operator!=(Filter_iterator const& other) const -> bool
    {
        return it_ != other.it_;
    }

    template <typename T>
    [[nodiscard]] auto operator==(T const& other) const -> bool
    {
        return it_ == other;
    }

    template <typename T>
    [[nodiscard]] auto operator!=(T const& other) const -> bool
    {
        return it_ != other;
    }

   private:
    Iter it_;
    Iter_end const end_;
    F predicate_;

   private:
    void increment_if_invalid()
    {
        while ((it_ != end_) && !predicate_(*it_))
            ++it_;
    }

    void increment()
    {
        do {
            ++it_;
        } while ((it_ != end_) && !predicate_(*it_));
    }
};

}  // namespace ox
#endif  // TERMOX_COMMON_FILTER_ITERATOR_HPP

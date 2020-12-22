#ifndef TERMOX_COMMON_RANGE_HPP
#define TERMOX_COMMON_RANGE_HPP
namespace ox {

/// Holds [begin, end) half-open range of iterators and provides access.
template <typename Iter_1, typename Iter_2>
struct Range {
   public:
    Range(Iter_1 begin, Iter_2 end) : begin_{begin}, end_{end} {}

   public:
    /// Return the iterator to the beginning of the Range.
    auto begin() const { return begin_; }

    /// Return the iterator to the end of the Range.
    auto end() const { return end_; }

   private:
    Iter_1 begin_;
    Iter_2 end_;
};

}  // namespace ox
#endif  // TERMOX_COMMON_RANGE_HPP

#ifndef CPPURSES_COMMON_RANGE_HPP
#define CPPURSES_COMMON_RANGE_HPP
namespace cppurses {

/// Holds [begin, end) half-open range of iterators and provides access.
template <typename Iter_1, typename Iter_2>
struct Range {
   public:
    Range(Iter_1 begin, Iter_2 end) : begin_{begin}, end_{end} {}

    /// Return the iterator to the beginning of the Range.
    auto begin() const { return begin_; }

    /// Return the iterator to the end of the Range.
    auto end() const { return end_; }

   private:
    Iter_1 begin_;
    Iter_1 end_;
};

/// Provides Widget_t& access to underlying child objects.
// template <typename Widget_t>
// class Range {
//     // TODO std::iterator is deprecated in C++17, as above
//     class Iterator
//         : public std::iterator<std::bidirectional_iterator_tag,
//                                typename List_t::iterator::value_type> {
//        public:
//         Iterator(List_t::iterator iter) : iter_{iter} {}
//         auto operator*() const -> Widget_t&
//         {
//             return static_cast<Widget_t&>(**iter_);
//         }
//         auto operator++() -> Iterator&
//         {
//             ++iter_;
//             return *this;
//         }
//         auto operator--() -> Iterator&
//         {
//             --iter_;
//             return *this;
//         }
//         auto operator==(Iterator other) const -> bool
//         {
//             return this->iter_ == other.iter_;
//         }
//         auto operator!=(Iterator other) const -> bool
//         {
//             return this->iter_ != other.iter_;
//         }

//        private:
//         List_t::iterator iter_;
//     };

//    public:
//     Range(List_t& child_list) : child_list_{child_list} {}

//     auto begin() const -> Iterator { return {std::begin(child_list_)}; }

//     auto end() const -> Iterator { return {std::end(child_list_)}; }

//     auto empty() const -> bool { return child_list_.empty(); }

//     auto size() const -> std::size_t { return child_list_.size(); }

//     auto operator[](std::size_t index) const -> Widget_t&
//     {
//         return static_cast<Widget_t&>(*child_list_[index]);
//     }

//     auto front() const -> Widget_t&
//     {
//         return static_cast<Widget_t&>(*child_list_.front());
//     }

//     auto back() const -> Widget_t&
//     {
//         return static_cast<Widget_t&>(*child_list_.back());
//     }

//    private:
//     List_t& child_list_;
// };
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_RANGE_HPP

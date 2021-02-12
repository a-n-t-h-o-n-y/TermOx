#ifndef TERMOX_COMMON_UNIQUE_QUEUE_HPP
#define TERMOX_COMMON_UNIQUE_QUEUE_HPP
#include <algorithm>
#include <cassert>
#include <iterator>
#include <utility>
#include <vector>

#include <termox/common/transform_iterator.hpp>

namespace ox {

/// A Queue like container holding only unique values.
/** Before accessing contained values, the container must be compressed to
 *  remove duplicates in a single step. This container has a call order
 *  dependency with compress, append, and clear! Uniqueness is determined by
 *  T::operator< and T::operator==. */
template <typename T>
class Unique_queue {
   private:
    struct Ordered_element {
        int position;
        T element;
    };

    /// Used in Transform_iterator
    class Get_element {
       public:
        auto operator()(Ordered_element const& e) const -> T const&
        {
            return e.element;
        }

        auto operator()(Ordered_element& e) const -> T& { return e.element; }
    };

    using Internal_container_t = std::vector<Ordered_element>;

   public:
    using iterator = Transform_iterator<typename Internal_container_t::iterator,
                                        Get_element>;

    using const_iterator =
        Transform_iterator<typename Internal_container_t::const_iterator,
                           Get_element>;

   public:
    /// Add \p element to the end of the queue.
    void append(T const& element)
    {
        assert(!compressed_but_not_cleared_);
        data_.push_back({count_, element});
        ++count_;
    }

    /// Add \p element to the end of the queue.
    void append(T&& element)
    {
        assert(!compressed_but_not_cleared_);
        data_.push_back({count_, std::move(element)});
        ++count_;
    }

    [[nodiscard]] auto size() const -> std::size_t { return data_.size(); }

    /// Remove duplicate elements, this should be called before using iterators.
    /** Do not append() after calling compress() until clear() has been
     *  called. compress and clear must be paired together, with iterator access
     *  in allowed in-between. */
    void compress()
    {
        std::stable_sort(
            std::begin(data_), std::end(data_),
            [](Ordered_element const& a, Ordered_element const& b) {
                return a.element < b.element;
            });
        auto rend =
            std::unique(std::rbegin(data_), std::rend(data_),
                        [](Ordered_element const& a, Ordered_element const& b) {
                            return a.element == b.element;
                        });
        data_.erase(std::begin(data_), rend.base());
        std::sort(std::begin(data_), std::end(data_),
                  [](Ordered_element const& a, Ordered_element const& b) {
                      return a.position < b.position;
                  });
#ifndef NDEBUG
        compressed_but_not_cleared_ = true;
#endif
    }

    /// Remove all elements from the queue.
    void clear()
    {
        data_.clear();
        count_ = 0;
#ifndef NDEBUG
        compressed_but_not_cleared_ = false;
#endif
    }

    [[nodiscard]] auto begin() -> iterator
    {
        return iterator{std::begin(data_), get_element_};
    }

    [[nodiscard]] auto begin() const -> const_iterator
    {
        return const_iterator{std::cbegin(data_), get_element_};
    }

    [[nodiscard]] auto end() -> iterator
    {
        return iterator{std::end(data_), get_element_};
    }

    [[nodiscard]] auto end() const -> const_iterator
    {
        return const_iterator{std::cend(data_), get_element_};
    }

   private:
    Internal_container_t data_;
    Get_element get_element_;
    int count_ = 0;
#ifndef NDEBUG
    bool compressed_but_not_cleared_ = false;
#endif
};
}  // namespace ox
#endif  // TERMOX_COMMON_UNIQUE_QUEUE_HPP

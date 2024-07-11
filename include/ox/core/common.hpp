#pragma once

#include <concepts>
#include <coroutine>
#include <iterator>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>

namespace ox {

/**
 * Implements the 'overload' pattern, useful for creating lambda overload sets.
 */
template <class... Ts>
struct Overload : Ts... {
    using Ts::operator()...;
};

/**
 * Applies \p fn to each element in \p t.
 */
template <typename... Ts, typename Fn>
void for_each(std::tuple<Ts...>& t, Fn&& fn)
{
    std::apply([&](auto&... x) { (std::forward<Fn>(fn)(x), ...); }, t);
}

/**
 * Applies \p fn to each element in \p t. const.
 */
template <typename... Ts, typename Fn>
void for_each(std::tuple<Ts...> const& t, Fn&& fn)
{
    std::apply([&](auto const&... x) { (std::forward<Fn>(fn)(x), ...); }, t);
}

/**
 * @brief Generator coroutine type that yields values of type T.
 */
template <typename T>
class Generator {
   public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        std::conditional_t<std::is_reference_v<T>,
                           std::remove_reference_t<T>*,
                           std::optional<T>>
            current_value;
        std::exception_ptr exception;

        auto get_return_object() { return Generator{handle_type::from_promise(*this)}; }

        auto initial_suspend() noexcept { return std::suspend_always{}; }

        auto final_suspend() noexcept { return std::suspend_always{}; }

        auto yield_value(T value) noexcept
        {
            if constexpr (std::is_reference_v<T>) {
                current_value = std::addressof(value);
            }
            else {
                current_value = std::move(value);
            }
            return std::suspend_always{};
        }

        void return_void() noexcept {}

        void unhandled_exception() { exception = std::current_exception(); }
    };

   public:
    explicit Generator(handle_type h) : handle_(h) {}

    Generator(Generator const&) = delete;
    auto operator=(Generator const&) -> Generator& = delete;

    Generator(Generator&& other) noexcept : handle_(other.handle_)
    {
        other.handle_ = nullptr;
    }

    auto operator=(Generator&& other) noexcept -> Generator&
    {
        if (this != &other) {
            if (handle_)
                handle_.destroy();
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }

    ~Generator()
    {
        if (handle_) {
            handle_.destroy();
        }
    }

    template <bool IsConst>
    class IteratorBase {
       public:
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = std::remove_reference_t<T>;
        using reference = std::conditional_t<IsConst, const T, T>;
        using pointer = std::conditional_t<IsConst, const value_type*, value_type*>;

        IteratorBase() noexcept = default;
        explicit IteratorBase(handle_type handle) : handle_(handle)
        {
            if (handle_) {
                handle_.resume();
                if (handle_.done()) {
                    handle_ = nullptr;
                }
            }
        }

        auto operator++() -> IteratorBase&
        {
            if (handle_ && !handle_.done()) {
                handle_.resume();
                if (handle_.done()) {
                    handle_ = nullptr;
                }
                else if (handle_.promise().exception) {
                    std::rethrow_exception(handle_.promise().exception);
                }
            }
            return *this;
        }

        auto operator++(int) -> IteratorBase
        {
            IteratorBase tmp = *this;
            ++(*this);
            return tmp;
        }

        auto operator*() const -> reference
        {
            if (!handle_ || handle_.done()) {
                throw std::out_of_range("Generator is exhausted");
            }
            if constexpr (std::is_reference_v<T>) {
                return *handle_.promise().current_value;
            }
            else {
                return *handle_.promise().current_value;
            }
        }

        auto operator->() const -> pointer { return std::addressof(operator*()); }

        auto operator==(const IteratorBase& other) const noexcept -> bool
        {
            return handle_ == other.handle_;
        }

        auto operator!=(const IteratorBase& other) const noexcept -> bool
        {
            return !(*this == other);
        }

       private:
        handle_type handle_;
    };

    using Iterator = IteratorBase<false>;
    using ConstIterator = IteratorBase<true>;

   public:
    auto begin() -> Iterator { return Iterator{handle_}; }
    auto begin() const -> ConstIterator { return ConstIterator{handle_}; }
    auto cbegin() const -> ConstIterator { return ConstIterator{handle_}; }

    auto end() -> Iterator { return Iterator{}; }
    auto end() const -> ConstIterator { return ConstIterator{}; }
    auto cend() const -> ConstIterator { return ConstIterator{}; }

   private:
    handle_type handle_;
};

/**
 * @brief Concept for an input range of T.
 */
template <typename R, typename T>
concept InputRangeOf =
    std::ranges::input_range<R> && std::same_as<std::ranges::range_value_t<R>, T>;

/**
 * @brief zip_iterator to iterate over two ranges simultaneously
 *
 * @tparam Iter1 first iterator type
 * @tparam Iter2 second iterator type
 */
template <typename Iter1, typename Iter2>
class zip_iterator {
    Iter1 iter1;
    Iter2 iter2;

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::pair<typename std::iterator_traits<Iter1>::value_type,
                                 typename std::iterator_traits<Iter2>::value_type>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = std::pair<typename std::iterator_traits<Iter1>::reference,
                                typename std::iterator_traits<Iter2>::reference>;

    constexpr zip_iterator(Iter1 it1, Iter2 it2) noexcept : iter1{it1}, iter2{it2} {}

    [[nodiscard]] constexpr auto operator*() const -> reference
    {
        return {*iter1, *iter2};
    }

    constexpr auto operator++() noexcept -> zip_iterator&
    {
        ++iter1;
        ++iter2;
        return *this;
    }

    [[nodiscard]] constexpr auto operator++(int) noexcept -> zip_iterator
    {
        auto temp = *this;
        ++*this;
        return temp;
    }

    [[nodiscard]] constexpr auto operator==(zip_iterator const& other) const noexcept
        -> bool
    {
        return iter1 == other.iter1 || iter2 == other.iter2;
    }

    [[nodiscard]] constexpr auto operator!=(zip_iterator const& other) const noexcept
        -> bool
    {
        return !(*this == other);
    }
};

/**
 * @brief zip_view to create a range from two ranges
 *
 * @tparam Range1 first range type
 * @tparam Range2 second range type
 */
template <typename Range1, typename Range2>
class zip_view : public std::ranges::view_interface<zip_view<Range1, Range2>> {
    Range1 range1;
    Range2 range2;

   public:
    constexpr zip_view(Range1 r1, Range2 r2) noexcept(
        std::is_nothrow_move_constructible_v<Range1> &&
        std::is_nothrow_move_constructible_v<Range2>)
        : range1{std::move(r1)}, range2{std::move(r2)}
    {}

    template <bool Const>
    class iterator {
        using Base1 = std::conditional_t<Const, const Range1, Range1>;
        using Base2 = std::conditional_t<Const, const Range2, Range2>;

       public:
        using Iter1 = std::ranges::iterator_t<Base1>;
        using Iter2 = std::ranges::iterator_t<Base2>;

        constexpr iterator(Iter1 it1, Iter2 it2) : it_{std::move(it1), std::move(it2)}
        {}

        constexpr auto operator*() const
        {
            return std::pair<std::ranges::range_reference_t<Base1>,
                             std::ranges::range_reference_t<Base2>>{*it_.first,
                                                                    *it_.second};
        }

        constexpr iterator& operator++()
        {
            ++it_.first;
            ++it_.second;
            return *this;
        }

        constexpr void operator++(int) { ++*this; }

        constexpr iterator operator++(int)
            requires std::forward_iterator<Iter1> && std::forward_iterator<Iter2>
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        friend constexpr bool operator==(const iterator& x, const iterator& y)
            requires std::equality_comparable<Iter1> && std::equality_comparable<Iter2>
        {
            return x.it_.first == y.it_.first || x.it_.second == y.it_.second;
        }

       private:
        std::pair<Iter1, Iter2> it_;
    };

    constexpr auto begin()
    {
        return iterator<false>{std::ranges::begin(range1), std::ranges::begin(range2)};
    }
    constexpr auto begin() const
    {
        return iterator<true>{std::ranges::begin(range1), std::ranges::begin(range2)};
    }
    constexpr auto end()
    {
        return iterator<false>{std::ranges::end(range1), std::ranges::end(range2)};
    }
    constexpr auto end() const
    {
        return iterator<true>{std::ranges::end(range1), std::ranges::end(range2)};
    }
};

/**
 * @brief Deduction guide for zip_view
 */
template <class R1, class R2>
zip_view(R1&&, R2&&) -> zip_view<std::views::all_t<R1>, std::views::all_t<R2>>;

/**
 * @brief zip function to create a zip_view from two ranges
 */
struct zip_fn {
    template <typename Range1, typename Range2>
    [[nodiscard]] constexpr auto operator()(Range1&& r1, Range2&& r2) const
        noexcept(noexcept(zip_view{std::forward<Range1>(r1), std::forward<Range2>(r2)}))
            -> zip_view<std::views::all_t<Range1>, std::views::all_t<Range2>>
    {
        return zip_view{std::forward<Range1>(r1), std::forward<Range2>(r2)};
    }
};

inline constexpr zip_fn zip{};

// -------------------------------------------------------------------------------------

/**
 * Pairs elements with an index.
 */
template <std::ranges::range R>
[[nodiscard]] auto enumerate(R&& range, std::size_t init = 0)
    -> Generator<std::pair<std::size_t, std::ranges::range_reference_t<R>>>
{
    for (auto&& e : std::forward<R>(range)) {
        co_yield (std::pair{init++, e});
    }
}

}  // namespace ox
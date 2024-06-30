#pragma once

#include <coroutine>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <tuple>
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
        std::optional<T> current_value;
        std::exception_ptr exception;

        auto get_return_object() { return Generator{handle_type::from_promise(*this)}; }

        auto initial_suspend() noexcept { return std::suspend_always{}; }

        auto final_suspend() noexcept { return std::suspend_always{}; }

        auto yield_value(T value) noexcept
        {
            current_value = std::move(value);
            return std::suspend_always{};
        }

        void return_void() noexcept {}

        void unhandled_exception() { exception = std::current_exception(); }
    };

   public:
    explicit Generator(handle_type h) : handle_(h) {}

    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;

    Generator(Generator&& other) noexcept : handle_(other.handle_)
    {
        other.handle_ = nullptr;
    }

    Generator& operator=(Generator&& other) noexcept
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
        if (handle_)
            handle_.destroy();
    }

    class Iterator {
       public:
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using reference = T const&;
        using pointer = T const*;

       public:
        Iterator() noexcept = default;
        explicit Iterator(handle_type handle) : handle_(handle)
        {
            if (handle_) {
                handle_.resume();
                if (handle_.done()) {
                    handle_ = nullptr;
                }
            }
        }

        auto operator++() -> Iterator&
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

        auto operator*() const -> reference
        {
            if (!handle_ || handle_.done() || !handle_.promise().current_value) {
                throw std::out_of_range("Generator is exhausted");
            }
            return *handle_.promise().current_value;
        }

        auto operator->() const -> pointer { return &(operator*()); }

        auto operator==(const Iterator& other) const -> bool noexcept
        {
            return handle_ == other.handle_;
        }

        auto operator!=(const Iterator& other) const -> bool noexcept
        {
            return !(*this == other);
        }

       private:
        handle_type handle_;
    };

   public:
    auto begin() -> Iterator { return Iterator{handle_}; }

    auto end() -> Iterator { return Iterator{}; }

   private:
    handle_type handle_;
};

}  // namespace ox
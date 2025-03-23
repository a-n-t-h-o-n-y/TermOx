#pragma once

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <memory>
#include <numeric>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <vector>

#include <zzz/coro.hpp>

#include <ox/bordered.hpp>
#include <ox/core/core.hpp>
#include <ox/widget.hpp>

namespace ox::detail {

/**
 * Calculate the length of each Widget in the given total length based on the its
 * SizePolicy and the total space available.
 *
 * @param widgets The widgets to distribute the space between.
 * @param total_length The total space to distribute.
 * @return A vector of the lengths of each child.
 */
template <InputRangeOf<Widget> WidgetRange>
[[nodiscard]]
auto distribute_length(WidgetRange&& widgets, int total_length) -> std::vector<int>
{
    assert(total_length >= 0);

    // Materialize the range because it can only be iterated once.
    // I'd like to use ranges filter + transform + to<vector> but must wait for C++23.
    auto const size_policies = [&] {
        auto result = std::vector<SizePolicy>{};
        for (auto const& w : widgets) {
            if (w.active) { result.push_back(w.size_policy); }
        }
        return result;
    }();

    for ([[maybe_unused]] auto const& policy : size_policies) {
        assert(policy.minimum >= 0);
        assert(policy.maximum >= policy.minimum);
        assert(policy.flexibility >= 0);
    }

    auto exact_amounts = std::vector<float>(size_policies.size(), 0.f);
    auto total_allocated = 0.f;

    // Distribute minimums first

    for (auto i = std::size_t{0}; i < size_policies.size(); ++i) {
        auto& exact_amount = exact_amounts[i];
        exact_amount = (float)size_policies[i].minimum;
        total_allocated += exact_amount;
    }

    // TODO
    // I do not believe this removes space from widgets if the sum of mins is greater
    // than the total space available.
    // You could have a check here.
    // if (total_allocated >= total) {
    //     auto results = std::vector<int>(policy_count, 0);

    //     // floor values to ints.
    //     std::ranges::copy(exact_amounts, results.begin());
    //     return results;
    // }

    // Distribute flex space
    auto remaining_space = (float)total_length - total_allocated;
    while (remaining_space > 0) {
        auto const total_flex = [&] {
            auto x = 0.f;
            for (auto i = std::size_t{0}; i < size_policies.size(); ++i) {
                if (exact_amounts[i] < (float)size_policies[i].maximum) {
                    x += size_policies[i].flexibility;
                }
            }
            return x;
        }();

        if (total_flex == 0.f) { break; }

        auto space_distributed_this_round = 0.f;
        for (auto i = std::size_t{0}; i < size_policies.size(); ++i) {
            if (exact_amounts[i] >= (float)size_policies[i].maximum) { continue; }

            float const additional_space =
                std::min(size_policies[i].flexibility / total_flex * remaining_space,
                         (float)size_policies[i].maximum - exact_amounts[i]);
            exact_amounts[i] += additional_space;
            space_distributed_this_round += additional_space;
        }
        remaining_space -= space_distributed_this_round;
        if (space_distributed_this_round == 0) { break; }
    }
    auto results = std::vector<int>(size_policies.size(), 0);

    // floor values to ints.
    std::ranges::copy(exact_amounts, results.begin());

    // Distribute remaining space from left from flooring.
    int remaining =
        total_length - std::accumulate(std::begin(results), std::end(results), 0);
    while (remaining > 0) {
        auto space_distributed_this_round = 0;
        for (auto i = std::size_t{0}; i < size_policies.size(); ++i) {
            auto& result = results[i];
            auto const& size_policy = size_policies[i];
            if (result < size_policy.maximum) {
                result += 1;
                remaining -= 1;
                ++space_distributed_this_round;
                if (remaining == 0) { break; }
            }
        }
        if (space_distributed_this_round == 0) { break; }
    }
    return results;
}

}  // namespace ox::detail

namespace ox {

/**
 * tuple, pair, array, etc...
 */
template <typename T>
concept TupleLike = requires {
    { std::tuple_size<T>::value } -> std::convertible_to<std::size_t>;
};

/**
 * vector, set, list, etc...
 */
template <typename T>
concept DynamicContainer = not TupleLike<std::remove_reference_t<T>> &&
                           std::ranges::range<T> && requires(T& container) {
                               { container.size() } -> std::convertible_to<std::size_t>;
                               { container.begin() } -> std::forward_iterator;
                               { container.end() } -> std::forward_iterator;
                           };

/**
 * A Vertical Linear Layout.
 *
 * @details This can take either tuple-like or range-like containers of Widgets. It will
 * order them with the first element on the top and the last on the bottom.
 */
template <typename Container>
class Column : public Widget {
    static_assert(TupleLike<Container> || DynamicContainer<Container>);

   public:
    Container children;

   public:
    // This is needed for Clang 18, instead of default value.
    template <typename C = Container>
        requires std::is_default_constructible_v<C>
    Column() : Widget{FocusPolicy::None, SizePolicy::flex()}, children{}
    {}

    Column(Container container)
        : Widget{FocusPolicy::None, SizePolicy::flex()}, children{std::move(container)}
    {}

    /**
     * Takes a function which can access each child to set up signals or anything else
     * needed after construction. If the container is tuple-like it will pass each child
     * Widget as a parameter, if it is a dynamic container it will pass the entire
     * container as parameter.
     */
    template <typename Fn>
    Column(Container container, Fn&& init_fn)
        : Widget{FocusPolicy::None, SizePolicy::flex()}, children{std::move(container)}
    {
        if constexpr (TupleLike<Container>) {
            std::apply(std::forward<Fn>(init_fn), children);
        }
        else {
            std::forward<Fn>(init_fn)(children);
        }
    }

   public:
    auto get_children() -> zzz::Generator<Widget&> override
    {
        if constexpr (TupleLike<Container>) {
            // Re-emitting values from apply so that this scope is a coroutine
            for (auto& w : std::apply(
                     [](auto&... child) -> zzz::Generator<Widget&> {
                         (co_yield child, ...);
                     },
                     children)) {
                co_yield w;
            }
        }
        else {
            for (Widget& child : children) {
                co_yield child;
            }
        }
    }

    auto get_children() const -> zzz::Generator<Widget const&> override
    {
        if constexpr (TupleLike<Container>) {
            // Re-emitting values from apply so that this scope is a coroutine
            for (auto const& w : std::apply(
                     [](auto const&... child) -> zzz::Generator<Widget const&> {
                         (co_yield child, ...);
                     },
                     children)) {
                co_yield w;
            }
        }
        else {
            for (auto const& child : children) {
                co_yield child;
            }
        }
    }

    void resize(Area) override
    {
        // TODO distribute_length filters out active children, can you do that in this
        // scope instead?
        auto const heights =
            detail::distribute_length(this->get_children(), this->size.height);

        auto active_children = this->get_children() | filter::is_active;

        auto y = 0;
        auto i = 0;
        for (auto& child : active_children) {
            child.at = {0, y};
            auto const old_size = child.size;
            child.size = {this->size.width, heights[i]};
            child.resize(old_size);
            y += heights[i];
            ++i;
        }
    }
};

/**
 * A Horizontal Linear Layout.
 *
 * @details This can take either tuple-like or range-like containers of Widgets. It will
 * order them with the first element on the left and the last on the right.
 */
template <typename Container>
class Row : public Widget {
    static_assert(TupleLike<Container> || DynamicContainer<Container>);

   public:
    Container children;

   public:
    // This is needed for Clang 18, instead of default value.
    template <typename C = Container>
        requires std::is_default_constructible_v<C>
    Row() : Widget{FocusPolicy::None, SizePolicy::flex()}, children{}
    {}

    Row(Container container)
        : Widget{FocusPolicy::None, SizePolicy::flex()}, children{std::move(container)}
    {}

    template <typename Fn>
    Row(Container container, Fn&& init_fn)
        : Widget{FocusPolicy::None, SizePolicy::flex()}, children{std::move(container)}
    {
        if constexpr (TupleLike<Container>) {
            std::apply(std::forward<Fn>(init_fn), children);
        }
        else {
            std::forward<Fn>(init_fn)(children);
        }
    }

   public:
    auto get_children() -> zzz::Generator<Widget&> override
    {
        if constexpr (TupleLike<Container>) {
            // Re-emitting values from apply so that this scope is a coroutine
            for (auto& w : std::apply(
                     [](auto&... child) -> zzz::Generator<Widget&> {
                         (co_yield child, ...);
                     },
                     children)) {
                co_yield w;
            }
        }
        else {
            for (Widget& child : children) {
                co_yield child;
            }
        }
    }

    auto get_children() const -> zzz::Generator<Widget const&> override
    {
        if constexpr (TupleLike<Container>) {
            // Re-emitting values from apply so that this scope is a coroutine
            for (auto const& w : std::apply(
                     [](auto const&... child) -> zzz::Generator<Widget const&> {
                         (co_yield child, ...);
                     },
                     children)) {
                co_yield w;
            }
        }
        else {
            for (auto const& child : children) {
                co_yield child;
            }
        }
    }

    void resize(Area) override
    {
        // TODO distribute_length filters out active children, can you do that in this
        // scope instead?
        auto const widths =
            detail::distribute_length(this->get_children(), this->size.width);

        auto active_children = this->get_children() | filter::is_active;

        auto x = 0;
        auto i = 0;
        for (auto& child : active_children) {
            child.at = {x, 0};
            auto const old_size = child.size;
            child.size = {widths[i], this->size.height};
            child.resize(old_size);
            x += widths[i];
            ++i;
        }
    }
};

}  // namespace ox
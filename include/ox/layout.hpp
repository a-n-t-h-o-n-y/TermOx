#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>
#include <numeric>
#include <ranges>
#include <tuple>
#include <vector>

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
[[nodiscard]] auto distribute_length(WidgetRange&& widgets,
                                     int total_length) -> std::vector<int>
{
    assert(total_length >= 0);

    // Materialize the range because it can only be iterated once.
    // I'd like to use ranges filter + transform + to<vector> but must wait for C++23.
    auto const size_policies = [&] {
        auto result = std::vector<SizePolicy>{};
        for (auto const& w : widgets) {
            if (w.active) {
                result.push_back(w.size_policy);
            }
        }
        return result;
    }();

    for (auto const& policy : size_policies) {
        assert(policy.minimum >= 0);
        assert(policy.maximum >= policy.minimum);
        assert(policy.flexibility >= 0);
    }

    auto exact_amounts = std::vector<float>(size_policies.size(), 0.f);
    auto total_allocated = 0.f;

    // Distribute minimums first
    for (auto [exact_amount, size_policy] : zip(exact_amounts, size_policies)) {
        exact_amount = (float)size_policy.minimum;
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
            for (auto [exact_amount, size_policy] : zip(exact_amounts, size_policies)) {
                if (exact_amount < (float)size_policy.maximum) {
                    x += size_policy.flexibility;
                }
            }
            return x;
        }();

        if (total_flex == 0.f) {
            break;
        }

        auto space_distributed_this_round = 0.f;
        for (auto [exact_amount, size_policy] : zip(exact_amounts, size_policies)) {
            if (exact_amount < (float)size_policy.maximum) {
                float additional_space =
                    std::min(size_policy.flexibility / total_flex * remaining_space,
                             (float)size_policy.maximum - exact_amount);
                exact_amount += additional_space;
                space_distributed_this_round += additional_space;
            }
        }
        remaining_space -= space_distributed_this_round;
        if (space_distributed_this_round == 0) {
            break;
        }
    }
    auto results = std::vector<int>(size_policies.size(), 0);

    // floor values to ints.
    std::ranges::copy(exact_amounts, results.begin());

    // Distribute remaining space from left from flooring.
    int remaining =
        total_length - std::accumulate(std::begin(results), std::end(results), 0);
    while (remaining > 0) {
        auto space_distributed_this_round = 0;
        for (auto [result, size_policy] : zip(results, size_policies)) {
            if (result < size_policy.maximum) {
                result += 1;
                remaining -= 1;
                ++space_distributed_this_round;
                if (remaining == 0) {
                    break;
                }
            }
        }
        if (space_distributed_this_round == 0) {
            break;
        }
    }
    return results;
}

// To lessen code duplication.
template <typename ContainerLayout>
struct HorizontalLayout : ContainerLayout {
    using ContainerLayout::ContainerLayout;

    void resize(Area new_size) override
    {
        auto const widths =
            detail::distribute_length(this->get_children(), new_size.width);

        auto x = 0;
        for (auto [child, width] :
             zip(this->get_children() | filter::is_active, widths)) {
            child.at = {x, 0};
            child.size = {width, new_size.height};
            child.resize({width, new_size.height});
            x += width;
        }
    }
};

// To lessen code duplication.
template <typename ContainerLayout>
struct VerticalLayout : ContainerLayout {
    using ContainerLayout::ContainerLayout;

    void resize(Area new_size) override
    {
        auto const heights =
            detail::distribute_length(this->get_children(), new_size.height);

        auto y = 0;
        for (auto [child, height] :
             zip(this->get_children() | filter::is_active, heights)) {
            child.at = {0, y};
            child.size = {new_size.width, height};
            child.resize(child.size);
            y += height;
        }
    }
};

}  // namespace ox::detail

namespace ox {

/**
 * A layout that arranges child widgets in a horizontal or vertical line, using
 * Widget::size_policy to determine the size of each child widget. Children Widgets are
 * added/removed at runtime.
 */
template <WidgetDerived WidgetBase>
class VectorLayout : public Widget {
   public:
    /**
     * Append a child widget to the layout.
     *
     * @param child The child widget to append.
     * @return A reference to the appended child widget.
     */
    template <WidgetDerived ChildType>
    auto append(ChildType child) -> ChildType&
    {
        children_.push_back(std::make_unique<ChildType>(std::move(child)));
        this->resize(this->Widget::size);
        return *static_cast<ChildType*>(children_.back().get());
    }

    template <WidgetDerived ChildType>
    auto append(Bordered<ChildType> bordered) -> std::pair<ChildType&, Border&>
    {
        children_.push_back(std::make_unique<Bordered<ChildType>>(std::move(bordered)));
        this->resize(this->Widget::size);
        auto& inplace = *static_cast<Bordered<ChildType>*>(children_.back().get());
        return {inplace.child, inplace.border};
    }

    template <WidgetDerived ChildType>
    auto insert_at(std::size_t index, ChildType child) -> ChildType&
    {
        children_.insert(
            std::next(std::begin(children_), static_cast<std::ptrdiff_t>(index)),
            std::make_unique<ChildType>(std::move(child)));
        this->resize(this->Widget::size);
        return *static_cast<ChildType*>(children_[index].get());
    }

    template <WidgetDerived ChildType>
    auto insert_at(std::size_t index,
                   Bordered<ChildType> child) -> std::pair<ChildType&, Border&>
    {
        children_.insert(
            std::next(std::begin(children_), static_cast<std::ptrdiff_t>(index)),
            std::make_unique<Bordered<ChildType>>(std::move(child)));
        this->resize(this->Widget::size);
        auto& inplace = *static_cast<Bordered<ChildType>*>(children_[index].get());
        return {inplace.child, inplace.border};
    }

    auto remove_at(std::size_t index) -> std::unique_ptr<WidgetBase>
    {
        auto removed = std::move(children_[index]);
        children_.erase(
            std::next(std::begin(children_), static_cast<std::ptrdiff_t>(index)));
        this->resize(this->Widget::size);
        return removed;
    }

    auto clear() -> void { children_.clear(); }

   public:
    auto get_children() -> Generator<Widget&> override
    {
        for (std::unique_ptr<WidgetBase> const& child_ptr : children_) {
            co_yield *child_ptr;
        }
    }

    auto get_children() const -> Generator<Widget const&> override
    {
        for (std::unique_ptr<WidgetBase> const& child_ptr : children_) {
            co_yield *child_ptr;
        }
    }

    [[nodiscard]] auto get_size_policies() const -> std::vector<SizePolicy>
    {
        auto result = std::vector<SizePolicy>{};
        for (auto const& child : children_) {
            result.push_back(child->size_policy);
        }
        return result;
    }

   private:
    std::vector<std::unique_ptr<WidgetBase>> children_;
};

/**
 * A layout that arranges child widgets in a horizontal line, using Widget::size_policy
 * to determine the size of each child widget. Children Widgets are added/removed at
 * runtime.
 */
template <WidgetDerived WidgetBase>
using HVector = detail::HorizontalLayout<VectorLayout<WidgetBase>>;

/**
 * A layout that arranges child widgets in a vertical line, using Widget::size_policy to
 * determine the size of each child widget. Children Widgets are added/removed at
 * runtime.
 */
template <WidgetDerived WidgetBase>
using VVector = detail::VerticalLayout<VectorLayout<WidgetBase>>;

// -------------------------------------------------------------------------------------

/**
 * A layout that arranges child widgets in a horizontal or vertical line. Children
 * Widgets are determined at compile-time.
 */
template <WidgetDerived... Widgets>
class TupleLayout : public Widget {
   public:
    std::tuple<Widgets...> children;

   public:
    TupleLayout() = default;

    TupleLayout(Widgets... children_) : children{std::move(children_)...} {}

   public:
    auto get_children() -> Generator<Widget&> override
    {
        return std::apply(
            [](auto&... child) -> Generator<Widget&> { (co_yield child, ...); },
            children);
    }

    auto get_children() const -> Generator<Widget const&> override
    {
        return std::apply(
            [](auto const&... child) -> Generator<Widget const&> {
                (co_yield child, ...);
            },
            children);
    }

    [[nodiscard]] auto get_size_policies() const -> std::vector<SizePolicy>
    {
        return std::apply(
            [](auto const&... child) -> std::vector<SizePolicy> {
                return {child.size_policy...};
            },
            children);
    }
};

/**
 * A layout that arranges child widgets in a horizontal line, using Widget::size_policy
 * to determine the size of each child widget. Children Widgets are determined at
 * compile-time.
 */
template <WidgetDerived... Widgets>
using HTuple = detail::HorizontalLayout<TupleLayout<Widgets...>>;

template <WidgetDerived... Widgets>
using HLayout = HTuple<Widgets...>;

/**
 * A layout that arranges child widgets in a vertical line, using Widget::size_policy to
 * determine the size of each child widget. Children Widgets are determined at
 * compile-time.
 */
template <WidgetDerived... Widgets>
using VTuple = detail::VerticalLayout<TupleLayout<Widgets...>>;

template <WidgetDerived... Widgets>
using VLayout = VTuple<Widgets...>;

//--------------------------------------------------------------------------------------

// TODO
class GridLayout {};

}  // namespace ox
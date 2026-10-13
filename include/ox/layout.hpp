#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <memory>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <vector>

#include <zzz/coro.hpp>

#include <ox/bordered.hpp>
#include <ox/core/layout.hpp>
#include <ox/widget.hpp>

namespace ox {

namespace detail {
inline void resize_children(Widget& parent, bool horizontal)
{
    auto children = std::vector<Widget*>{};
    auto policies = std::vector<SizePolicy>{};
    for (auto& child : parent.get_children()) {
        if (!child.active) { continue; }
        children.push_back(&child);
        policies.push_back(child.size_policy);
    }
    auto const rectangles = horizontal ? layout_row(policies, parent.size)
                                       : layout_column(policies, parent.size);
    for (auto i = std::size_t{0}; i < children.size(); ++i) {
        auto& child = *children[i];
        auto const old_size = child.size;
        child.at = rectangles[i].at;
        child.size = rectangles[i].size;
        child.resize(old_size);
    }
}
}  // namespace detail

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
                           std::ranges::sized_range<T> && std::ranges::forward_range<T>;

template <typename T>
concept LayoutContainer = DynamicContainer<T> || TupleLike<T>;

/**
 * A Vertical Linear Layout.
 *
 * @details This can take either tuple-like or range-like containers of Widgets. It will
 * order them with the first element on the top and the last on the bottom.
 */
template <LayoutContainer Container>
class Column : public Widget {
   public:
    Container children;

   public:
    // This is needed for Clang 18, instead of default value.
    template <typename C = Container>
        requires std::is_default_constructible_v<C>
    explicit Column() : Widget{FocusPolicy::None, SizePolicy::flex()}, children{}
    {}

    explicit Column(Container container)
        : Widget{FocusPolicy::None, SizePolicy::flex()}, children{std::move(container)}
    {}

    template <WidgetDerived... Widgets>
    explicit Column(Widgets&&... ws)
        : Widget{FocusPolicy::None, SizePolicy::flex()},
          children{std::tuple{std::forward<Widgets>(ws)...}}
    {}

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

    void resize(Area) override { detail::resize_children(*this, false); }
};

template <LayoutContainer Container>
Column(Container) -> Column<Container>;

template <WidgetDerived... Widgets>
Column(Widgets&&...) -> Column<std::tuple<std::decay_t<Widgets>...>>;

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
    explicit Row() : Widget{FocusPolicy::None, SizePolicy::flex()}, children{}
    {}

    explicit Row(Container container)
        : Widget{FocusPolicy::None, SizePolicy::flex()}, children{std::move(container)}
    {}

    template <WidgetDerived... Widgets>
    explicit Row(Widgets&&... ws)
        : Widget{FocusPolicy::None, SizePolicy::flex()},
          children{std::tuple{std::forward<Widgets>(ws)...}}
    {}

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

    void resize(Area) override { detail::resize_children(*this, true); }
};

template <LayoutContainer Container>
Row(Container) -> Row<Container>;

template <WidgetDerived... Widgets>
Row(Widgets&&...) -> Row<std::tuple<std::decay_t<Widgets>...>>;

// -------------------------------------------------------------------------------------

template <WidgetDerived ChildType>
class Suspended : public Widget {
   public:
    ChildType child;
    Area requested_size;
    Glyph fill_glyph;

   public:
    explicit Suspended(ChildType&& child, Area requested, Glyph fill = {U' '})
        : Widget{FocusPolicy::None, SizePolicy::flex()},
          child{std::move(child)},
          requested_size{requested},
          fill_glyph{fill}
    { validate(requested_size); }

   public:
    void paint(Canvas c) override
    {
        // Paint the four margins through clipped child views.
        auto const right = child.at.x + child.size.width;
        auto const bottom = child.at.y + child.size.height;
        fill(c.subcanvas({0, 0}, {std::max(child.at.x, 0), c.size().height}),
             fill_glyph);
        fill(c.subcanvas({right, 0},
                         {std::max(c.size().width - right, 0), c.size().height}),
             fill_glyph);
        fill(c.subcanvas({child.at.x, 0}, {child.size.width, std::max(child.at.y, 0)}),
             fill_glyph);
        fill(c.subcanvas({child.at.x, bottom},
                         {child.size.width, std::max(c.size().height - bottom, 0)}),
             fill_glyph);
    }

    void resize(Area) override
    {
        auto const old_size = child.size;

        auto const rectangle = centered_rect(this->size, requested_size);
        child.size = rectangle.size;
        child.at = rectangle.at;

        child.resize(old_size);
    }

    auto get_children() -> zzz::Generator<Widget&> override { co_yield child; }

    auto get_children() const -> zzz::Generator<Widget const&> override
    { co_yield child; }
};

// -------------------------------------------------------------------------------------

template <std::size_t I, TupleLike Container>
auto get_child(Row<Container>& row) -> auto&
{ return std::get<I>(row.children); }

template <std::size_t I, TupleLike Container>
auto get_child(Column<Container>& row) -> auto&
{ return std::get<I>(row.children); }

template <WidgetDerived ChildType>
auto get_child(Suspended<ChildType>& sus) -> auto&
{ return sus.child; }

}  // namespace ox
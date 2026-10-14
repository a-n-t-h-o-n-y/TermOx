#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <memory>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <zzz/coro.hpp>

#include <ox/bordered.hpp>
#include <ox/core/layout.hpp>
#include <ox/widget.hpp>

namespace ox {

/** Mutable widget values/references in a tuple, pair, array, or sized forward range. */
template <typename T>
concept TupleLike = requires {
    { std::tuple_size<T>::value } -> std::convertible_to<std::size_t>;
};
template <typename T>
concept DynamicContainer = not TupleLike<std::remove_reference_t<T>> &&
                           std::ranges::sized_range<T> && std::ranges::forward_range<T>;
template <typename T>
concept LayoutContainer = DynamicContainer<T> || TupleLike<T>;

namespace detail {
template <LayoutContainer Container, typename Function>
void for_each_child(Container& container, Function function)
{
    if constexpr (TupleLike<Container>) {
        std::apply([&](auto&... child) { (function(child), ...); }, container);
    }
    else {
        for (auto& child : container) {
            function(child);
        }
    }
}

template <LayoutContainer Container>
auto child_references(Container& container) -> zzz::Generator<
    std::conditional_t<std::is_const_v<Container>, Widget const&, Widget&>>
{
    if constexpr (TupleLike<Container>) {
        // One coroutine frame, with tuple pointers stored in that frame.
        auto pointers = std::apply(
            [](auto&... child) {
                using Pointer = std::conditional_t<std::is_const_v<Container>,
                                                   Widget const*, Widget*>;
                return std::array<Pointer, sizeof...(child)>{std::addressof(child)...};
            },
            container);
        for (auto* child : pointers) {
            co_yield *child;
        }
    }
    else {
        for (auto& child : container) {
            co_yield child;
        }
    }
}

template <typename Container, bool Horizontal>
class LinearLayout : public Widget {
    static_assert(LayoutContainer<Container>);

   public:
    using Widget::handle;
    Container children;

    explicit LinearLayout()
        requires std::default_initializable<Container>
        : Widget{FocusPolicy::None, SizePolicy::flex()}, children{}
    {}
    explicit LinearLayout(Container container)
        : Widget{FocusPolicy::None, SizePolicy::flex()}, children{std::move(container)}
    {}
    template <WidgetDerived... Widgets>
        requires(sizeof...(Widgets) > 0)
    explicit LinearLayout(Widgets&&... widgets)
        : LinearLayout{Container{std::forward<Widgets>(widgets)...}}
    {}
    LinearLayout(LinearLayout&& other)
        : Widget{std::move(other)},
          children{std::move(other.children)},
          targets_{std::move(other.targets_)},
          target_storage_{std::move(other.target_storage_)},
          policies_{std::move(other.policies_)},
          policy_storage_{std::move(other.policy_storage_)},
          rectangles_{std::move(other.rectangles_)},
          available_{other.available_},
          arranging_{std::exchange(other.arranging_, false)},
          pending_{std::exchange(other.pending_, false)}
    { other.available_ = {-1, -1}; }
    auto operator=(LinearLayout&& other) -> LinearLayout&
    {
        if (this == &other) { return *this; }
        Widget::operator=(std::move(other));
        children = std::move(other.children);
        targets_ = std::move(other.targets_);
        target_storage_ = std::move(other.target_storage_);
        policies_ = std::move(other.policies_);
        policy_storage_ = std::move(other.policy_storage_);
        rectangles_ = std::move(other.rectangles_);
        available_ = std::exchange(other.available_, Area{-1, -1});
        arranging_ = std::exchange(other.arranging_, false);
        pending_ = std::exchange(other.pending_, false);
        return *this;
    }

    /** Call after editing children, visibility, or size policies. Arrange also always
     * refreshes descendants, even if this layout's geometry did not change. */
    void refresh() { arrange(*this, bounds()); }
    auto get_children() -> zzz::Generator<Widget&> override
    { return child_references(children); }
    auto get_children() const -> zzz::Generator<Widget const&> override
    { return child_references(children); }

   protected:
    void on_arrange(Rect) override
    {
        if (arranging_) {
            pending_ = true;
            return;
        }
        // Retain the identity slot, not the widget. Moves update it; destruction
        // clears it. Resolve again after every hook without repeated weak locks.
        auto lifetime = this->lifetime_;
        auto* previous = static_cast<Widget*>(this);
        auto* current = this;
        auto resolve = [&]() -> LinearLayout* {
            auto* next = lifetime ? *lifetime : nullptr;
            if (next != previous) {
                previous = next;
                current = dynamic_cast<LinearLayout*>(next);
            }
            return current;
        };
        arranging_ = true;
        try {
            auto first = true;
            for (;;) {
                auto* layout = resolve();
                if (!layout) { return; }
                if (!first && !layout->inputs_changed()) { break; }
                layout->prepare();
                layout->pending_ = false;
                first = false;
                auto restart = false;
                auto const count = layout->targets_.size();
                for (auto i = std::size_t{0}; i < count; ++i) {
                    layout = resolve();
                    if (!layout) { return; }
                    auto* child = layout->targets_[i] ? *layout->targets_[i] : nullptr;
                    auto const rectangle = layout->rectangles_[i];
                    if (child && child->visible) { arrange(*child, rectangle); }
                    layout = resolve();
                    if (!layout) { return; }
                    if (layout->pending_) {
                        restart = layout->inputs_changed();
                        layout->pending_ = false;
                        if (restart) { break; }
                    }
                }
                if (restart) { continue; }
                layout = resolve();
                if (!layout) { return; }
                if (!layout->inputs_changed()) { break; }
            }
        }
        catch (...) {
            if (auto* layout = resolve()) {
                layout->arranging_ = false;
                layout->pending_ = false;
            }
            throw;
        }
        if (auto* layout = resolve()) {
            layout->arranging_ = false;
            layout->pending_ = false;
        }
    }

   private:
    std::vector<std::shared_ptr<Widget*>> targets_;
    std::vector<std::shared_ptr<Widget*>> target_storage_;
    std::vector<SizePolicy> policies_;
    std::vector<SizePolicy> policy_storage_;
    std::vector<Rect> rectangles_;
    Area available_ = {-1, -1};
    bool arranging_ = false;
    bool pending_ = false;

    auto inputs_changed() const -> bool
    {
        if (available_ != size()) { return true; }
        auto index = std::size_t{0};
        auto changed = false;
        for_each_child(children, [&](Widget const& child) {
            if (!child.visible) { return; }
            changed |= index >= targets_.size() || targets_[index] != child.lifetime_ ||
                       child.size_policy != policies_[index];
            ++index;
        });
        return changed || index != targets_.size();
    }
    void prepare()
    {
        if (!inputs_changed()) { return; }
        policy_storage_.clear();
        auto index = std::size_t{0};
        auto membership_changed = false;
        for_each_child(children, [&](Widget& child) {
            if (!child.visible) { return; }
            policy_storage_.push_back(child.size_policy);
            membership_changed |=
                index >= targets_.size() || targets_[index] != child.lifetime_;
            ++index;
        });
        membership_changed |= index != targets_.size();
        if (membership_changed) {
            target_storage_.clear();
            for_each_child(children, [&](Widget& child) {
                if (child.visible) { target_storage_.push_back(child.lifetime_); }
            });
        }
        // Validation and allocation finish before calling any child layout hooks.
        if (available_ != size() || policy_storage_ != policies_) {
            auto rectangles = Horizontal ? layout_row(policy_storage_, size())
                                         : layout_column(policy_storage_, size());
            rectangles_.swap(rectangles);
            policies_.swap(policy_storage_);
            available_ = size();
        }
        if (membership_changed) { targets_.swap(target_storage_); }
    }
};
}  // namespace detail

/** Vertical/horizontal layout over an ordinary container of widgets. */
template <LayoutContainer Container>
class Column : public detail::LinearLayout<Container, false> {
   public:
    using detail::LinearLayout<Container, false>::LinearLayout;
};
template <LayoutContainer Container>
Column(Container) -> Column<Container>;
template <WidgetDerived... Widgets>
Column(Widgets&&...) -> Column<std::tuple<std::decay_t<Widgets>...>>;

template <LayoutContainer Container>
class Row : public detail::LinearLayout<Container, true> {
   public:
    using detail::LinearLayout<Container, true>::LinearLayout;
};
template <LayoutContainer Container>
Row(Container) -> Row<Container>;
template <WidgetDerived... Widgets>
Row(Widgets&&...) -> Row<std::tuple<std::decay_t<Widgets>...>>;

// -------------------------------------------------------------------------------------

template <WidgetDerived ChildType>
class Suspended : public Widget {
   public:
    using Widget::handle;

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
    auto paint(Canvas c) -> Cursor override
    {
        fill_outside(c, child.bounds(), fill_glyph);

        return {};
    }

   protected:
    void on_arrange(Rect) override { refresh(); }

   public:
    /** Recompute geometry after changing requested_size; notify even if unchanged. */
    void refresh() { arrange(child, centered_rect(size(), requested_size)); }

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

template <std::size_t I, TupleLike Container>
auto get_child(Row<Container> const& row) -> auto const&
{ return std::get<I>(row.children); }

template <std::size_t I, TupleLike Container>
auto get_child(Column<Container> const& column) -> auto const&
{ return std::get<I>(column.children); }

template <WidgetDerived ChildType>
auto get_child(Suspended<ChildType> const& suspended) -> auto const&
{ return suspended.child; }

template <WidgetDerived ChildType>
auto get_child(Suspended<ChildType>& sus) -> auto&
{ return sus.child; }

}  // namespace ox
/** @file
 * Rows, columns, padding, stacks, and suspending child layout.
 */
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

/** Tuple protocol used for fixed child storage, including tuples, pairs, and arrays. */
template <typename T>
concept TupleLike = requires {
    { std::tuple_size<T>::value } -> std::convertible_to<std::size_t>;
};
/** A sized forward range used as layout child storage, excluding tuple-like containers. */
template <typename T>
concept DynamicContainer = not TupleLike<std::remove_reference_t<T>> &&
                           std::ranges::sized_range<T> && std::ranges::forward_range<T>;
/** Tuple-like or sized forward-range child storage accepted by layouts, stacks, tabs,
 * and forms.
 */
template <typename T>
concept LayoutContainer = DynamicContainer<T> || TupleLike<T>;

namespace detail {
/** Invoke a function on each stored widget/reference in tuple or range order; the
 * container is borrowed during traversal.
 */
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

/** Yield borrowed immediate widget references in storage order; the container must
 * survive coroutine iteration.
 */
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

/** Shared Row/Column implementation over retained tuple or range storage.
 * Invisible children and overlays do not consume normal layout space. Axis policies
 * control allocation, gap separates visible children, and cross_align controls the
 * other axis. Call refresh() after changing storage or public layout configuration.
 */
template <typename Container, bool Horizontal>
class LinearLayout : public Widget {
    static_assert(LayoutContainer<Container>);

   public:
    using Widget::handle;
    /** Child storage retained by value; tuples of references borrow rather than own the
     * referenced widgets.
     */
    Container children;
    /** Nonnegative cells between adjacent children or field rows; call refresh() after
     * direct changes.
     */
    int gap = 0;
    /** Placement or stretching perpendicular to the layout's main axis. */
    CrossAlign cross_align = CrossAlign::Stretch;

    /** Create empty default-initialized child storage without taking focus. */
    explicit LinearLayout()
        requires std::default_initializable<Container>
        : Widget{FocusPolicy::None, SizePolicy::flex()}, children{}
    {}
    /** Retain child storage by value; tuples of references continue to borrow external
     * widgets.
     */
    explicit LinearLayout(Container container)
        : Widget{FocusPolicy::None, SizePolicy::flex()}, children{std::move(container)}
    {}
    /** Build tuple/range storage from forwarded child values or references. */
    template <WidgetDerived... Widgets>
        requires(sizeof...(Widgets) > 0)
    explicit LinearLayout(Widgets&&... widgets)
        : LinearLayout{Container{std::forward<Widgets>(widgets)...}}
    {}
    /** Transfer widget identity, retained state, and signal subscriptions; tracked
     * references follow the move.
     */
    LinearLayout(LinearLayout&& other)
        : Widget{std::move(other)},
          children{std::move(other.children)},
          gap{other.gap},
          cross_align{other.cross_align},
          targets_{std::move(other.targets_)},
          target_storage_{std::move(other.target_storage_)},
          policies_{std::move(other.policies_)},
          policy_storage_{std::move(other.policy_storage_)},
          rectangles_{std::move(other.rectangles_)},
          cross_{std::move(other.cross_)},
          gap_{other.gap_},
          cross_align_{other.cross_align_},
          available_{other.available_},
          arranging_{std::exchange(other.arranging_, false)},
          pending_{std::exchange(other.pending_, false)}
    { other.available_ = {-1, -1}; }
    /** Replace this widget's identity and retained state with the source; previous
     * lifetime views of this identity expire.
     */
    auto operator=(LinearLayout&& other) -> LinearLayout&
    {
        if (this == &other) { return *this; }
        Widget::operator=(std::move(other));
        children = std::move(other.children);
        gap = other.gap;
        cross_align = other.cross_align;
        cross_ = std::move(other.cross_);
        gap_ = other.gap_;
        cross_align_ = other.cross_align_;
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

    auto measure(Area available, esc::WidthPolicy policy = {}) const -> Area override
    {
        validate(available);
        if (gap < 0) { throw std::invalid_argument{"Negative layout gap"}; }
        auto primary = std::int64_t{0};
        auto secondary = 0;
        auto count = 0;
        for_each_child(children, [&](Widget const& child) {
            if (!child.visible || child.overlay) { return; }
            auto natural = child.measure(available, policy);
            validate(natural);
            auto main = Horizontal ? natural.width : natural.height;
            auto cross = Horizontal ? natural.height : natural.width;
            auto main_policy = Horizontal ? child.width_policy : child.height_policy;
            auto cross_policy = Horizontal ? child.height_policy : child.width_policy;
            main =
                fit_length(main, main_policy, std::numeric_limits<int>::max(), false);
            cross =
                fit_length(cross, cross_policy, std::numeric_limits<int>::max(), false);
            primary += main + (count++ ? (std::int64_t)gap : 0);
            secondary = std::max(secondary, cross);
        });
        if (primary > std::numeric_limits<int>::max()) {
            throw std::length_error{"Layout extent overflow"};
        }
        return Horizontal ? Area{(int)primary, secondary}
                          : Area{secondary, (int)primary};
    }

    /** Call after editing children, visibility, or size policies. Arrange also always
     * refreshes descendants, even if this layout's geometry did not change. */
    void refresh() override
    {
        request_layout();
        arrange(*this, bounds());
    }
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
    std::vector<std::pair<Area, SizePolicy>> cross_;
    int gap_ = -1;
    CrossAlign cross_align_ = CrossAlign::Stretch;
    Area available_ = {-1, -1};
    bool arranging_ = false;
    bool pending_ = false;

    auto inputs(Widget const& child) const
        -> std::pair<SizePolicy, std::pair<Area, SizePolicy>>
    {
        (void)track(child);
        auto available = size();
        available.width =
            fit_length(available.width, child.width_policy, available.width, true);
        auto natural = child.measure(available, glyph_width_policy());
        validate(natural);
        auto main = Horizontal ? child.width_policy : child.height_policy;
        auto const extent = Horizontal ? natural.width : natural.height;
        (void)fit_length(extent, main, std::numeric_limits<int>::max(), false);
        main.minimum = std::max(main.minimum, std::min(extent, main.maximum));
        return {main, {natural, Horizontal ? child.height_policy : child.width_policy}};
    }
    auto inputs_changed() const -> bool
    {
        if (available_ != size() || gap_ != gap || cross_align_ != cross_align) {
            return true;
        }
        auto index = std::size_t{0};
        auto changed = false;
        for_each_child(children, [&](Widget const& child) {
            if (!child.visible || child.overlay) { return; }
            auto const [main, cross] = inputs(child);
            changed |= index >= targets_.size() || targets_[index] != child.lifetime_ ||
                       main != policies_[index] || cross != cross_[index];
            ++index;
        });
        return changed || index != targets_.size();
    }
    void prepare()
    {
        if (!inputs_changed()) { return; }
        policy_storage_.clear();
        auto next_cross = std::vector<std::pair<Area, SizePolicy>>{};
        target_storage_.clear();
        for_each_child(children, [&](Widget& child) {
            if (!child.visible || child.overlay) { return; }
            auto [main, cross] = inputs(child);
            policy_storage_.push_back(main);
            next_cross.push_back(cross);
            target_storage_.push_back(child.lifetime_);
        });
        auto rectangles = Horizontal ? layout_row(policy_storage_, size(), gap)
                                     : layout_column(policy_storage_, size(), gap);
        auto const cross_available = Horizontal ? size().height : size().width;
        for (std::size_t i = 0; i < rectangles.size(); ++i) {
            auto const [natural, policy] = next_cross[i];
            auto const extent =
                fit_length(Horizontal ? natural.height : natural.width, policy,
                           cross_available, cross_align == CrossAlign::Stretch);
            auto offset = 0;
            switch (cross_align) {
                case CrossAlign::Stretch:
                case CrossAlign::Start: break;
                case CrossAlign::Center: offset = (cross_available - extent) / 2; break;
                case CrossAlign::End: offset = cross_available - extent; break;
                default: throw std::invalid_argument{"Invalid cross alignment"};
            }
            if constexpr (Horizontal) {
                rectangles[i].at.y = offset;
                rectangles[i].size.height = extent;
            }
            else {
                rectangles[i].at.x = offset;
                rectangles[i].size.width = extent;
            }
        }
        rectangles_.swap(rectangles);
        policies_.swap(policy_storage_);
        cross_.swap(next_cross);
        targets_.swap(target_storage_);
        available_ = size();
        gap_ = gap;
        cross_align_ = cross_align;
    }
};
}  // namespace detail

/** Vertical/horizontal layout over an ordinary container of widgets. */
template <LayoutContainer Container>
class Column : public detail::LinearLayout<Container, false> {
   public:
    using detail::LinearLayout<Container, false>::LinearLayout;
};
/** Deduce column storage from the supplied container. */
template <LayoutContainer Container>
Column(Container) -> Column<Container>;
/** Deduce owned tuple storage from individual column child arguments. */
template <WidgetDerived... Widgets>
Column(Widgets&&...) -> Column<std::tuple<std::decay_t<Widgets>...>>;

/** Lay out visible non-overlay children horizontally, with width policies controlling
 * growth and cross_align controlling vertical placement.
 */
template <LayoutContainer Container>
class Row : public detail::LinearLayout<Container, true> {
   public:
    using detail::LinearLayout<Container, true>::LinearLayout;
};
/** Deduce row storage from the supplied container. */
template <LayoutContainer Container>
Row(Container) -> Row<Container>;
/** Deduce owned tuple storage from individual row child arguments. */
template <WidgetDerived... Widgets>
Row(Widgets&&...) -> Row<std::tuple<std::decay_t<Widgets>...>>;

// -------------------------------------------------------------------------------------

/** Center one retained child at a requested size and paint a fill glyph outside its
 * clipped bounds.
 */
template <WidgetDerived ChildType>
class Suspended : public Widget {
   public:
    using Widget::handle;

    /** Retained child widget centered within the available area. */
    ChildType child;
    /** Requested child extent; dimensions must be nonnegative. Call refresh() after
     * changing it.
     */
    Area requested_size;
    /** Background glyph used by Suspended when painting its unused area. */
    Glyph fill_glyph;

   public:
    /** Own the child and a nonnegative requested extent; oversized children are
     * constrained to the available area.
     */
    explicit Suspended(ChildType&& child, Area requested, Glyph fill = {U' '})
        : Widget{FocusPolicy::None, SizePolicy::flex()},
          child{std::move(child)},
          requested_size{requested},
          fill_glyph{fill}
    { validate(requested_size); }

   public:
    auto measure(Area available, esc::WidthPolicy = {}) const -> Area override
    {
        validate(available);
        validate(requested_size);
        return requested_size;
    }
    auto paint(Canvas c) -> Cursor override
    {
        fill_outside(c, child.bounds(), fill_glyph);

        return {};
    }

   protected:
    void on_arrange(Rect) override
    { arrange(child, centered_rect(size(), requested_size)); }

   public:
    /** Recompute geometry after changing requested_size; notify even if unchanged. */
    void refresh() override
    {
        request_layout();
        arrange(child, centered_rect(size(), requested_size));
    }

    auto get_children() -> zzz::Generator<Widget&> override { co_yield child; }

    auto get_children() const -> zzz::Generator<Widget const&> override
    { co_yield child; }
};

// -------------------------------------------------------------------------------------

/** Borrow the Ith stored row child; I is a compile-time tuple index. */
template <std::size_t I, TupleLike Container>
auto get_child(Row<Container>& row) -> auto&
{ return std::get<I>(row.children); }

/** Borrow the Ith stored column child; I is a compile-time tuple index. */
template <std::size_t I, TupleLike Container>
auto get_child(Column<Container>& row) -> auto&
{ return std::get<I>(row.children); }

/** Borrow the Ith stored row child read-only. */
template <std::size_t I, TupleLike Container>
auto get_child(Row<Container> const& row) -> auto const&
{ return std::get<I>(row.children); }

/** Borrow the Ith stored column child read-only. */
template <std::size_t I, TupleLike Container>
auto get_child(Column<Container> const& column) -> auto const&
{ return std::get<I>(column.children); }

/** Borrow the centered child read-only. */
template <WidgetDerived ChildType>
auto get_child(Suspended<ChildType> const& suspended) -> auto const&
{ return suspended.child; }

/** Borrow the centered child without changing ownership. */
template <WidgetDerived ChildType>
auto get_child(Suspended<ChildType>& sus) -> auto&
{ return sus.child; }

}  // namespace ox
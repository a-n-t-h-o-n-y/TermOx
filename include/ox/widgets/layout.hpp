#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#include <ox/widgets/widget.hpp>

namespace ox::widgets {
struct Divider;

/**
 * Policy for how a widget should be sized by its parent layout.
 */
struct SizePolicy {
    int minimum = 0;
    int maximum = std::numeric_limits<int>::max();
    float flexibility = 1.f;

    [[nodiscard]] static auto fixed(int size) -> SizePolicy;
    [[nodiscard]] static auto flex(float flex = 1.f) -> SizePolicy;
    [[nodiscard]] static auto bounded(int min, int max) -> SizePolicy;
    [[nodiscard]] static auto min(int min) -> SizePolicy;
    [[nodiscard]] static auto max(int max) -> SizePolicy;
};

/**
 * A layout that arranges its children in a line, either horizontally or vertically. Do
 * not use directly, instead use HLayout or VLayout.
 */
struct LinearLayout {
    std::vector<Widget> children = {};
    std::vector<SizePolicy> size_policies = {};

    template <typename... Widgets>
    explicit LinearLayout(Widgets&&... children_)
    {
        static_assert((!std::is_same_v<std::remove_cvref_t<Widgets>, Widget> && ...),
                      "`Widget` type should not be passed as an argument");

        size_policies.resize(sizeof...(children_), SizePolicy{});

        (children.emplace_back(std::forward<Widgets>(children_)), ...);
    }
};

inline auto children(LinearLayout& w) -> std::span<Widget> { return w.children; }

inline auto children(LinearLayout const& w) -> std::span<Widget const>
{
    return w.children;
}

// -------------------------------------------------------------------------------------

/**
 * Append a Widget to the LinearLayout.
 *
 * @param t The Widget to append.
 * @param size_policy The size policy to apply to the Widget.
 * @param focus_policy The focus policy to apply to the Widget.
 * @return A reference to the appended Widget. This reference will remain valid until
 * the Widget is destroyed.
 */
template <typename T>
auto append(LinearLayout& layout,
            T t,
            SizePolicy size_policy = {},
            FocusPolicy focus_policy = FocusPolicy::None) -> T&
{
    static_assert(!std::is_same_v<std::remove_cvref_t<T>, Widget>);

    layout.size_policies.push_back(size_policy);

    return layout.children.emplace_back(std::move(t), focus_policy).template data<T>();
}

/**
 * Inserts a Widget into the LinearLayout at the given index.
 *
 * @param index The index to insert the Widget at. If this is greater than the current
 * number of children, the Widget will be appended to the end of the layout.
 * @param t The Widget to insert.
 * @param size_policy The size policy to apply to the Widget.
 * @param focus_policy The focus policy to apply to the Widget.
 * @return A reference to the inserted Widget. This reference will remain valid until
 * the Widget is destroyed.
 */
template <typename T>
auto insert_at(LinearLayout& layout,
               std::size_t index,
               T t,
               SizePolicy size_policy = {},
               FocusPolicy focus_policy = FocusPolicy::None) -> T&
{
    static_assert(!std::is_same_v<std::remove_cvref_t<T>, Widget>);

    if (index >= layout.children.size()) {
        return append(layout, std::move(t), size_policy, focus_policy);
    }

    layout.size_policies.insert(
        std::next(std::begin(layout.size_policies), (std::ptrdiff_t)index),
        size_policy);

    return layout.children
        .emplace(std::next(std::begin(layout.children), index), std::move(t),
                 focus_policy)
        ->template data<T>();
}

/**
 * Removes and returns the given Widget from the LinearLayout.
 *
 * @details Find \p w with find_if_depth_first.
 * @param layout The LinearLayout to remove \p w from.
 * @param w The Widget to remove.
 * @return The removed Widget.
 * @throws std::out_of_range If \p w is not found in \p layout.
 */
auto remove(LinearLayout& layout, Widget const& w) -> Widget;

/**
 * Removes and returns the Widget at the given index from the LinearLayout.
 *
 * @param layout The LinearLayout to remove the Widget from.
 * @param index The index of the Widget to remove.
 * @return The removed Widget.
 * @throws std::out_of_range If \p index is greater than or equal to the number of
 * children in \p layout.
 */
auto remove_at(LinearLayout& layout, std::size_t index) -> Widget;

/**
 * Removes and returns all Widgets from the LinearLayout.
 *
 * @param layout The LinearLayout to remove all Widgets from.
 * @return A vector containing all the removed Widgets.
 */
auto remove_all(LinearLayout& layout) -> std::vector<Widget>;

// -------------------------------------------------------------------------------------

struct HLayout : LinearLayout {
    using LinearLayout::LinearLayout;
};

auto resize(HLayout& layout, Area a) -> void;

auto append_divider(HLayout& layout, Glyph line = {U'│'}) -> Divider&;

// -------------------------------------------------------------------------------------

struct VLayout : LinearLayout {
    using LinearLayout::LinearLayout;
};

auto resize(VLayout& layout, ox::Area a) -> void;

auto append_divider(VLayout& layout, Glyph line = {U'─'}) -> Divider&;

// -------------------------------------------------------------------------------------

// TODO
struct GridLayout {};

}  // namespace ox::widgets
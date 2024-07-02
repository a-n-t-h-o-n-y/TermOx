#pragma once

#include <memory>
#include <tuple>
#include <vector>

#include <ox/core/core.hpp>
#include <ox/widget.hpp>

namespace ox {

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

// -------------------------------------------------------------------------------------

template <typename WidgetBase>
struct LinearLayoutDynamic : Widget {
    // TODO add a make<> type function that check the type inherits from widgetbase or
    // is widgetbase and inherits from Widget, then make this a class and make children
    // hidden. Then provide an insert and remove set of fns to work with vector.
    // separate remove and erase functions? erase would .reset() and remove would
    // return?
    std::vector<std::unique_ptr<WidgetBase>> children;

    auto get_children() -> Generator<Widget&> override
    {
        for (WidgetBase& child_ptr : children) {
            co_yield *child_ptr;
        }
    }

    auto get_children() const -> Generator<Widget const&> override
    {
        for (WidgetBase const& child_ptr : children) {
            co_yield *child_ptr;
        }
    }
};

template <typename... Widgets>
struct LinearLayout : Widget {
    std::tuple<Widgets...> children;
    // TODO what does construction look like and signal hooking up?
    // Can it be deduced? not if you use it as a base class

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
};

// -------------------------------------------------------------------------------------

template <typename... Widgets>
struct HLayout : LinearLayout<Widgets...> {
    void resize(Area new_size) override
    {
        // just evenly distribute
        // auto const total_flex = std::apply(
        //     [](auto const&... child) {
        //         return (child.size_policy.flexibility + ...);
        //     },
        //     this->children);

        auto const count = sizeof...(Widgets);
        auto const uniform_height = new_size.height / count;
        auto const width = new_size.width;

        std::apply(
            [uniform_height, width, at = 0](auto&... child) mutable {
                // assign child.size.height and width, then send resize event
                // do this in lambda?

                (
                    [&at](auto& child, int height, int width) mutable {
                        child.size = {width, height};
                        child.at = {0, at};
                        at += height;
                        child.resize({width, height});
                    }(child, uniform_height, width),
                    ...);
            },
            this->children);
    }
};

// struct Divider;

// /**
//  * Policy for how a widget should be sized by its parent layout.
//  */
// struct SizePolicy {
//     int minimum = 0;
//     int maximum = std::numeric_limits<int>::max();
//     float flexibility = 1.f;

//     [[nodiscard]] static auto fixed(int size) -> SizePolicy;
//     [[nodiscard]] static auto flex(float flex = 1.f) -> SizePolicy;
//     [[nodiscard]] static auto bounded(int min, int max) -> SizePolicy;
//     [[nodiscard]] static auto min(int min) -> SizePolicy;
//     [[nodiscard]] static auto max(int max) -> SizePolicy;
// };

// /**
//  * A layout that arranges its children in a line, either horizontally or vertically.
//  Do
//  * not use directly, instead use HLayout or VLayout.
//  */
// struct LinearLayout {
//     std::vector<Widget> children = {};
//     std::vector<SizePolicy> size_policies = {};

//     template <typename... Widgets>
//     explicit LinearLayout(Widgets&&... children_)
//     {
//         static_assert((!std::is_same_v<std::remove_cvref_t<Widgets>, Widget> && ...),
//                       "`Widget` type should not be passed as an argument");

//         size_policies.resize(sizeof...(children_), SizePolicy{});

//         (children.emplace_back(std::forward<Widgets>(children_)), ...);
//     }

//     /**
//      * Append a Widget to the LinearLayout.
//      *
//      * @param t The Widget to append.
//      * @param size_policy The size policy to apply to the Widget.
//      * @param focus_policy The focus policy to apply to the Widget.
//      * @return A reference to the appended Widget. This reference will remain valid
//      * until the Widget is destroyed.
//      */
//     template <typename T>
//     auto append(T t,
//                 SizePolicy size_policy = {},
//                 FocusPolicy focus_policy = FocusPolicy::None) -> T&
//     {
//         static_assert(!std::is_same_v<std::remove_cvref_t<T>, Widget>);

//         this->size_policies.push_back(size_policy);

//         return this->children.emplace_back(std::move(t), focus_policy)
//             .template data<T>();
//     }
// };

// inline auto children(LinearLayout& w) -> std::span<Widget> { return w.children; }

// inline auto children(LinearLayout const& w) -> std::span<Widget const>
// {
//     return w.children;
// }

////-------------------------------------------------------------------------------------

// /**
//  * Inserts a Widget into the LinearLayout at the given index.
//  *
//  * @param index The index to insert the Widget at. If this is greater than the
//  current
//  * number of children, the Widget will be appended to the end of the layout.
//  * @param t The Widget to insert.
//  * @param size_policy The size policy to apply to the Widget.
//  * @param focus_policy The focus policy to apply to the Widget.
//  * @return A reference to the inserted Widget. This reference will remain valid until
//  * the Widget is destroyed.
//  */
// template <typename T>
// auto insert_at(LinearLayout& layout,
//                std::size_t index,
//                T t,
//                SizePolicy size_policy = {},
//                FocusPolicy focus_policy = FocusPolicy::None) -> T&
// {
//     static_assert(!std::is_same_v<std::remove_cvref_t<T>, Widget>);

//     if (index >= layout.children.size()) {
//         return layout.append(std::move(t), size_policy, focus_policy);
//     }

//     layout.size_policies.insert(
//         std::next(std::begin(layout.size_policies), (std::ptrdiff_t)index),
//         size_policy);

//     return layout.children
//         .emplace(std::next(std::begin(layout.children), index), std::move(t),
//                  focus_policy)
//         ->template data<T>();
// }

// /**
//  * Removes and returns the given Widget from the LinearLayout.
//  *
//  * @details Find \p w with find_if_depth_first.
//  * @param layout The LinearLayout to remove \p w from.
//  * @param w The Widget to remove.
//  * @return The removed Widget.
//  * @throws std::out_of_range If \p w is not found in \p layout.
//  */
// auto remove(LinearLayout& layout, Widget const& w) -> Widget;

// /**
//  * Removes and returns the Widget at the given index from the LinearLayout.
//  *
//  * @param layout The LinearLayout to remove the Widget from.
//  * @param index The index of the Widget to remove.
//  * @return The removed Widget.
//  * @throws std::out_of_range If \p index is greater than or equal to the number of
//  * children in \p layout.
//  */
// auto remove_at(LinearLayout& layout, std::size_t index) -> Widget;

// /**
//  * Removes and returns all Widgets from the LinearLayout.
//  *
//  * @param layout The LinearLayout to remove all Widgets from.
//  * @return A vector containing all the removed Widgets.
//  */
// auto remove_all(LinearLayout& layout) -> std::vector<Widget>;

////-------------------------------------------------------------------------------------

// struct HLayout : LinearLayout {
//     using LinearLayout::LinearLayout;
// };

// template <typename... Widgets>
// [[nodiscard]] auto hlayout(Widgets&&... children) -> HLayout
// {
//     return {std::forward<Widgets>(children)...};
// }

// auto resize(HLayout& layout, Area a) -> void;

// auto append_divider(HLayout& layout, Glyph line = {U'│'}) -> Divider&;

////-------------------------------------------------------------------------------------

// struct VLayout : LinearLayout {
//     using LinearLayout::LinearLayout;
// };

// template <typename... Widgets>
// [[nodiscard]] auto vlayout(Widgets&&... children) -> VLayout
// {
//     return {std::forward<Widgets>(children)...};
// }

// auto resize(VLayout& layout, Area a) -> void;

// auto append_divider(VLayout& layout, Glyph line = {U'─'}) -> Divider&;

////-------------------------------------------------------------------------------------

// // TODO
// struct GridLayout {};

}  // namespace ox

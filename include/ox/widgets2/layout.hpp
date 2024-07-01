#pragma once

#include <memory>
#include <tuple>
#include <vector>

#include <ox/core.hpp>
#include <ox/widgets2/widget.hpp>

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

}  // namespace ox

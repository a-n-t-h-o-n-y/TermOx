#pragma once

#include <memory>
#include <tuple>
#include <type_traits>

#include <ox/core.hpp>

namespace ox {

/**
 * Policy for how a widget should be sized by its parent layout.
 */
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

// -------------------------------------------------------------------------------------

template <typename... Widgets>
struct LinearLayout {
    template <typename W>
    struct Data {
        std::unique_ptr<W> widget;
        Point at;
        Area size;
        bool enabled;
    };
    std::tuple<Data<Widgets>...> children;

    LinearLayout(Widgets... children)
        : children{Data<Widgets>{
              .widget = std::make_unique<Widgets>(std::move(children)),
              .at = {0, 0},
              .size = {0, 0},
              .enabled = true,
          }...}
    {}

    template <std::size_t I>
    auto& get()
    {
        return std::get<I>(children);
    }

    template <std::size_t I>
    auto const& get() const
    {
        return std::get<I>(children);
    }
};

// -------------------------------------------------------------------------------------

namespace detail {

inline auto enabled_and_has_point(auto& widget, Point p) -> bool
{
    return widget.enabled && p.x >= widget.at.x && p.y >= widget.at.y &&
           p.x < widget.at.x + widget.size.width &&
           p.y < widget.at.y + widget.size.height;
}

}  // namespace detail

// -------------------------------------------------------------------------------------

template <typename... Widgets>
void paint(LinearLayout<Widgets...> const& layout, Canvas canvas)
{
    for_each(layout.children, [&](auto const& child) {
        if (child.enabled && child.size.width > 0 && child.size.height > 0) {
            using Child_t = std::decay_t<decltype(*child.widget)>;
            if constexpr (requires(Child_t w, Canvas c) { paint(w, c); }) {
                paint(*(child.widget), Canvas{
                                           .buffer = canvas.buffer,
                                           .at = canvas.at + child.at,
                                           .size = child.size,
                                       });
            }
        }
    });
}

template <typename... Widgets>
void mouse_press(LinearLayout<Widgets...>& layout, Mouse m)
{
    // TODO set focus
    for_each(layout.children, [&](auto& child) {
        if (detail::enabled_and_has_point(child, m.at)) {
            using Child_t = std::decay_t<decltype(*child.widget)>;
            if constexpr (requires(Child_t w, Mouse m) { mouse_press(w, m); }) {
                mouse_press(*(child.widget), Mouse{
                                                 .at = m.at - child.at,
                                                 .button = m.button,
                                             });
            }
        }
    });
}

template <typename... Widgets>
void mouse_release(LinearLayout<Widgets...>& layout, Mouse m)
{
    for_each(layout.children, [&](auto& child) {
        if (detail::enabled_and_has_point(child, m.at)) {
            using Child_t = std::decay_t<decltype(*child.widget)>;
            if constexpr (requires(Child_t w, Mouse m) { mouse_release(w, m); }) {
                mouse_release(*(child.widget),
                              Mouse{.at = m.at - child.at, .button = m.button});
            }
        }
    });
}

template <typename... Widgets>
void mouse_wheel(LinearLayout<Widgets...>& layout, Mouse m)
{
    for_each(layout.children, [&](auto& child) {
        if (detail::enabled_and_has_point(child, m.at)) {
            using Child_t = std::decay_t<decltype(*child.widget)>;
            if constexpr (requires(Child_t w, Mouse m) { mouse_wheel(w, m); }) {
                mouse_wheel(*(child.widget),
                            Mouse{.at = m.at - child.at, .button = m.button});
            }
        }
    });
}

template <typename... Widgets>
void mouse_move(LinearLayout<Widgets...>& layout, Mouse m)
{
    // TODO implement leave/enter
    for_each(layout.children, [&](auto& child) {
        if (detail::enabled_and_has_point(child, m.at)) {
            using Child_t = std::decay_t<decltype(*child.widget)>;
            if constexpr (requires(Child_t w, Mouse m) { mouse_move(w, m); }) {
                mouse_move(*(child.widget),
                           Mouse{.at = m.at - child.at, .button = m.button});
            }
        }
    });
}

template <typename... Widgets>
void mouse_enter(LinearLayout<Widgets...>& layout)
{
    // TODO implement leave/enter
}

template <typename... Widgets>
void mouse_leave(LinearLayout<Widgets...>& layout)
{
    // TODO implement leave/enter
}

// TODO cursor?

template <typename... Widgets>
void key_press(LinearLayout<Widgets...>& layout, Key k)
{
    // TODO send to focus widget, if this is not part of the focus chain then do not
    // send to children
}

template <typename... Widgets>
void key_release(LinearLayout<Widgets...>& layout, Key k)
{
    // TODO send to focus widget, if this is not part of the focus chain then do not
    // send to children
}

template <typename... Widgets>
void focus_in(LinearLayout<Widgets...>& layout)
{
    // TODO Not sure if this is needed, maybe it is part of the focus change
    // implementation
}

template <typename... Widgets>
void focus_out(LinearLayout<Widgets...>& layout)
{
    // TODO Not sure if this is needed, maybe it is part of the focus change
    // implementation
}

// -------------------------------------------------------------------------------------

template <typename... Widgets>
struct HLayout : LinearLayout<Widgets...> {
    HLayout(Widgets... children) : LinearLayout<Widgets...>{std::move(children)...} {}
};

template <typename... Widgets>
void resize(HLayout<Widgets...>& layout, Area new_size)
{
    // TODO implement size policy
    auto const width = (int)(new_size.width / sizeof...(Widgets));

    for_each(layout.children, [&, x = 0](auto& child) mutable {
        child.at = {x, 0};
        x += width;
        child.size = Area{width, new_size.height};
        using Child_t = std::decay_t<decltype(*child.widget)>;
        if constexpr (requires(Child_t w, Area a) { resize(w, a); }) {
            resize(*(child.widget), child.size);
        }
    });
}

// -------------------------------------------------------------------------------------

template <typename... Widgets>
struct VLayout : LinearLayout<Widgets...> {
    VLayout(Widgets... children) : LinearLayout<Widgets...>{std::move(children)...} {}
};

template <typename... Widgets>
void resize(VLayout<Widgets...>& layout, Area new_size)
{
    // TODO implement size policy
    auto const height = (int)(new_size.height / sizeof...(Widgets));

    for_each(layout.children, [&, y = 0](auto& child) mutable {
        child.at = {0, y};
        y += height;
        child.size = Area{new_size.width, height};
        using Child_t = std::decay_t<decltype(*child.widget)>;
        if constexpr (requires(Child_t w, Area a) { resize(w, a); }) {
            resize(*(child.widget), child.size);
        }
    });
}

}  // namespace ox
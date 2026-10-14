#pragma once

#include <algorithm>
#include <utility>

#include <ox/align.hpp>
#include <ox/core/core.hpp>
#include <ox/label.hpp>
#include <ox/put.hpp>
#include <ox/widget.hpp>

namespace ox {

struct Border {
    shape::Box box = shape::Box::square();
    Style style = {};  // Only applies to box.
    Label::Options label = {};
};

/** Interior geometry is valid even when the border consumes the entire area. */
[[nodiscard]] auto border_content_rect(Area available) -> Rect;
/** Draw the border and its title; leave the interior untouched. */
void paint_border(Canvas canvas, Border const& border);

/**
 * A Widget that wraps another Widget with a Border display.
 */
template <WidgetDerived ChildWidget>
class Bordered : public Widget {
   public:
    using Widget::handle;
    struct Options {
        ChildWidget child = {};
        Border border = {};
    };

   public:
    ChildWidget child;
    Border border;

    Bordered(Options x)
        : Widget{FocusPolicy::None, SizePolicy::flex()},
          child{std::move(x.child)},
          border{std::move(x.border)}
    { arrange(child, border_content_rect(size())); }

   protected:
    void on_arrange(Rect) override { arrange(child, border_content_rect(size())); }

   public:
    auto measure(Area available, esc::WidthPolicy policy = {}) const -> Area override
    {
        auto const natural = child.measure(border_content_rect(available).size, policy);
        validate(natural);
        auto const result = translate({natural.width, natural.height}, {2, 2});
        return {result.x, result.y};
    }
    auto paint(Canvas canvas) -> Cursor override
    {
        auto config = border;
        config.style = appearance(config.style, Surface::Border);
        config.label.style = appearance(config.label.style, Surface::Accent);
        if (theme().styled && theme().ascii) config.box = shape::Box::ascii();
        paint_border(canvas, config);
        return {};
    }

    auto get_children() -> zzz::Generator<Widget&> override { co_yield child; }

    auto get_children() const -> zzz::Generator<Widget const&> override
    { co_yield child; }
};

// Moves \p w into a Bordered object
template <WidgetDerived WidgetType>
[[nodiscard]] auto operator|(WidgetType w, Border b) -> Bordered<WidgetType>
{ return {{.child = std::move(w), .border = std::move(b)}}; }

/**
 * Returns the `I`th child of the wrapped Widget. Useful if this is wrapping a Layout.
 */
template <std::size_t I, WidgetDerived WidgetType>
auto get_child(Bordered<WidgetType>& bordered) -> auto&
{ return get_child<I>(bordered.child); }

template <std::size_t I, WidgetDerived WidgetType>
auto get_child(Bordered<WidgetType> const& bordered) -> auto const&
{ return get_child<I>(bordered.child); }

// -------------------------------------------------------------------------------------

struct Lines {
    char32_t vertical;
    char32_t horizontal;

    [[nodiscard]] static constexpr auto light() -> Lines { return {U'│', U'─'}; }
    [[nodiscard]] static constexpr auto bold() -> Lines { return {U'┃', U'━'}; }
    [[nodiscard]] static constexpr auto twin() -> Lines { return {U'║', U'═'}; }
    [[nodiscard]] static constexpr auto dashed() -> Lines { return {U'╎', U'╌'}; }
    [[nodiscard]] static constexpr auto dotted() -> Lines { return {U'┆', U'┄'}; }
    [[nodiscard]] static constexpr auto ascii() -> Lines { return {U'|', U'-'}; }
    [[nodiscard]] static constexpr auto empty() -> Lines { return {U' ', U' '}; }
};

/** Clear the view, draw a centered line and overlay its label. */
void paint_divider(Canvas canvas,
                   bool vertical,
                   Lines lines,
                   Style style,
                   Label::Options const& label = {});

/// A line divider element.
class Divider : public Widget {
   public:
    using Widget::handle;

    struct Options {
        bool vertical = false;
        Lines lines = Lines::light();
        Style style = {};
        Label::Options label = {};
    } static const init;

   public:
    bool vertical;
    Lines lines;
    Style style;
    Label::Options label;

    Divider(Options x = init);

   public:
    auto paint(Canvas c) -> Cursor override;
};

}  // namespace ox
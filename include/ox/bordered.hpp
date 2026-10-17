/** @file
 * Border outlines, titles, and child decoration.
 */
#pragma once

#include <algorithm>
#include <utility>

#include <ox/align.hpp>
#include <ox/core/core.hpp>
#include <ox/label.hpp>
#include <ox/put.hpp>
#include <ox/widget.hpp>

namespace ox {

/** One-cell outline and optional title applied around a child without owning that child
 * itself.
 */
struct Border {
    /** Corner and wall glyphs used to draw the border outline. */
    shape::Box box = shape::Box::square();
    /** Base drawing style; explicit colors take precedence over theme defaults. */
    Style style = {};  // Only applies to box.
    /** Optional border/divider title with its own alignment and style. */
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
    /** Construction-time values; field initializers below define the defaults. */
    struct Options {
        /** Owned wrapped widget, arranged inside the border's one-cell inset. */
        ChildWidget child = {};
        /** Border shape, drawing style, and title configuration. */
        Border border = {};
    };

   public:
    /** Owned wrapped widget, arranged inside the border's one-cell inset. */
    ChildWidget child;
    /** Border shape, drawing style, and title configuration. */
    Border border;

    /** Own a wrapped child and border configuration; arrange the child inside a clipped
     * one-cell inset.
     */
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
/** Move a widget into an owning Bordered wrapper with the supplied outline and title. */
template <WidgetDerived WidgetType>
[[nodiscard]] auto operator|(WidgetType w, Border b) -> Bordered<WidgetType>
{ return {{.child = std::move(w), .border = std::move(b)}}; }

/**
 * Returns the `I`th child of the wrapped Widget. Useful if this is wrapping a Layout.
 */
template <std::size_t I, WidgetDerived WidgetType>
auto get_child(Bordered<WidgetType>& bordered) -> auto&
{ return get_child<I>(bordered.child); }

/** Borrow the Ith child of the wrapped layout read-only. */
template <std::size_t I, WidgetDerived WidgetType>
auto get_child(Bordered<WidgetType> const& bordered) -> auto const&
{ return get_child<I>(bordered.child); }

// -------------------------------------------------------------------------------------

/** Matching vertical and horizontal divider glyphs; presets preserve consistent stroke
 * weight.
 */
struct Lines {
    /** Scalar used for a vertical divider stroke. */
    char32_t vertical;
    /** Scalar used for a horizontal divider stroke. */
    char32_t horizontal;

    /** Thin single-line box-drawing strokes. */
    [[nodiscard]] static constexpr auto light() -> Lines { return {U'│', U'─'}; }
    /** Heavy box-drawing strokes. */
    [[nodiscard]] static constexpr auto bold() -> Lines { return {U'┃', U'━'}; }
    /** Double-line box-drawing strokes. */
    [[nodiscard]] static constexpr auto twin() -> Lines { return {U'║', U'═'}; }
    /** Dashed single-line strokes. */
    [[nodiscard]] static constexpr auto dashed() -> Lines { return {U'╎', U'╌'}; }
    /** Dotted single-line strokes. */
    [[nodiscard]] static constexpr auto dotted() -> Lines { return {U'┆', U'┄'}; }
    /** ASCII vertical bar and hyphen strokes. */
    [[nodiscard]] static constexpr auto ascii() -> Lines { return {U'|', U'-'}; }
    /** Space strokes for a divider with no visible line. */
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

    /** Construction-time values; field initializers below define the defaults. */
    struct Options {
        /** Draw a vertical divider when true; horizontal is the default. */
        bool vertical = false;
        /** Divider stroke glyphs; choose a Lines factory for a consistent pair. */
        Lines lines = Lines::light();
        /** Base drawing style; explicit colors take precedence over theme defaults. */
        Style style = {};
        /** Optional border/divider title with its own alignment and style. */
        Label::Options label = {};
    } static const init; ///< Default options used when no explicit configuration is supplied.

   public:
    /** Draw a vertical divider when true; horizontal is the default. */
    bool vertical;
    /** Divider stroke glyphs; choose a Lines factory for a consistent pair. */
    Lines lines;
    /** Base drawing style; explicit colors take precedence over theme defaults. */
    Style style;
    /** Optional border/divider title with its own alignment and style. */
    Label::Options label;

    /** Configure stroke orientation and label; choose size policies to reserve the
     * intended separator area.
     */
    Divider(Options x = init);

   public:
    auto paint(Canvas c) -> Cursor override;
};

}  // namespace ox
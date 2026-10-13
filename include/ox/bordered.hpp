#pragma once

#include <array>
#include <stdexcept>

#include <ox/align.hpp>
#include <ox/core/core.hpp>
#include <ox/label.hpp>
#include <ox/put.hpp>
#include <ox/widget.hpp>

namespace ox {

struct Border {
    shape::Box box = shape::Box::square();
    Style style = {};  // Only applies to box.
    Label label = {};
};

/**
 * A Widget that wraps another Widget with a Border display.
 */
template <WidgetDerived ChildWidget>
class Bordered : public Widget {
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
    { child.at = {1, 1}; }

   public:
    void resize(Area) override
    {
        auto const old_size = child.size;
        child.size = {
            .width = std::max(0, this->size.width - 2),
            .height = std::max(0, this->size.height - 2),
        };
        child.resize(old_size);
    }

    void paint(Canvas c) override
    {
        // Box
        put(c, border.box, border.style);

        // Label - Can't use Label::paint because it paints over border with bg color.
        auto& label = border.label;
        auto const width = std::max(0, c.size().width - 2);
        auto text = esc::utf8_to_utf32(label.text);
        auto length = std::size_t{0};
        auto columns = std::size_t{0};
        for (auto cluster : esc::graphemes(text)) {
            auto cells = esc::grapheme_width(cluster, c.width_policy());
            if (!cells || *cells == 0) {
                throw std::invalid_argument{"Border label requires printable text"};
            }
            if (*cells > (std::size_t)width - columns) { break; }
            columns += *cells;
            length += cluster.size();
        }
        auto const x = find_align_offset(label.align, width, (int)columns);
        put(c.subcanvas({1, 0}, {width, 1}), {x, 0},
            esc::glyphs(std::u32string_view{text}.substr(0, length), label.style));
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

// -------------------------------------------------------------------------------------

struct Lines {
    char32_t vertical;
    char32_t horizontal;

    [[nodiscard]] static constexpr auto light() -> Lines { return {U'│', U'─'}; }
    [[nodiscard]] static constexpr auto bold() -> Lines { return {U'┃', U'━'}; }
    [[nodiscard]] static constexpr auto twin() -> Lines { return {U'║', U'═'}; }
    [[nodiscard]] static constexpr auto dashed() -> Lines { return {U'╎', U'╌'}; }
    [[nodiscard]] static constexpr auto dotted() -> Lines { return {U'┆', U'┄'}; }
    [[nodiscard]] static constexpr auto ascii() -> Lines { return {U'-', U'|'}; }
    [[nodiscard]] static constexpr auto empty() -> Lines { return {U' ', U' '}; }
};

/// A line divider element.
class Divider : public Widget {
   public:
    struct Options {
        Lines lines = Lines::light();
        Style style = {};
        Label label = {};
    };

   public:
    Lines lines;
    Style style;
    Label label;  // TODO

    Divider(Options x);

   public:
    void paint(Canvas c) override;
};

}  // namespace ox
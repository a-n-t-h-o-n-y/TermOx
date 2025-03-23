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
    shape::Box box = {};
    Label label = {};

    struct Options {
        Color foreground = XColor::Default;
        Label label = {};
    };

    /// Light Border: ┌┐└┘─│
    [[nodiscard]] static auto light(std::string label = "") -> Border;
    [[nodiscard]] static auto light(Options x) -> Border;

    /// Round Border: ╭╮╰╯─│
    [[nodiscard]] static auto round(std::string label = "") -> Border;
    [[nodiscard]] static auto round(Options x) -> Border;

    /// Twin Border: ╔╗╚╝═║
    [[nodiscard]] static auto twin(std::string label = "") -> Border;
    [[nodiscard]] static auto twin(Options x) -> Border;

    /// Bold Border: ┏┓┗┛━┃
    [[nodiscard]] static auto bold(std::string label = "") -> Border;
    [[nodiscard]] static auto bold(Options x) -> Border;

    /// Dashed Border: ┌┐└┘╌╎
    [[nodiscard]] static auto dashed(std::string label = "") -> Border;
    [[nodiscard]] static auto dashed(Options x) -> Border;

    /// Dotted Border: ┌┐└┘┄┆
    [[nodiscard]] static auto dotted(std::string label = "") -> Border;
    [[nodiscard]] static auto dotted(Options x) -> Border;

    /// ASCII Border: ++++-|
    [[nodiscard]] static auto ascii(std::string label = "") -> Border;
    [[nodiscard]] static auto ascii(Options x) -> Border;

    /// Twin Horizontal Border: ╒╕╘╛═│
    [[nodiscard]] static auto twin_horizontal(std::string label = "") -> Border;
    [[nodiscard]] static auto twin_horizontal(Options x) -> Border;

    /// Twin Vertical Border: ╓╖╙╜─║
    [[nodiscard]] static auto twin_vertical(std::string label = "") -> Border;
    [[nodiscard]] static auto twin_vertical(Options x) -> Border;
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
    {
        child.at = {1, 1};
    }

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
        border.box.size = this->size;
        put(c, {.x = 0, .y = 0}, border.box);

        // Label - Can't use Label::paint because it paints over border with bg color.
        auto& label = border.label;
        auto const width = std::max(0, c.size.width - 2);
        auto const glyphs =
            std::string_view{label.text}.substr(0, (std::size_t)width) | label.brush;
        auto const at = Point{
            .x = 1 + find_align_offset(label.align, width, glyphs.size()),
            .y = 0,
        };
        put(c, at, glyphs);
    }

    auto get_children() -> zzz::Generator<Widget&> override { co_yield child; }

    auto get_children() const -> zzz::Generator<Widget const&> override { co_yield child; }
};

// Moves \p w into a Bordered object
template <WidgetDerived WidgetType>
[[nodiscard]] auto operator|(WidgetType w, Border b) -> Bordered<WidgetType>
{
    return {{.child = std::move(w), .border = std::move(b)}};
}

// -------------------------------------------------------------------------------------

/// A line divider element.
class Divider : public Widget {
   public:
    struct Display {
        char32_t vertical;
        char32_t horizontal;
    };

   private:
    struct Options {
        Display lines = {.vertical = U'│', .horizontal = U'─'};
        Brush brush = {};
        Label label = {};
    };

   public:
    Display lines;
    Brush brush;
    Label label;  // TODO

    Divider(Options x)
        : Widget{FocusPolicy::None, SizePolicy::fixed(1)},
          lines{std::move(x.lines)},
          brush{std::move(x.brush)},
          label{std::move(x.label)}
    {}

   public:
    void paint(Canvas c) override;

   public:
    /// Light Divider: ─│
    [[nodiscard]] static auto light(std::string label = "") -> Divider;
    [[nodiscard]] static auto light(Options x) -> Divider;

    /// Bold Divider: ━┃
    [[nodiscard]] static auto bold(std::string label = "") -> Divider;
    [[nodiscard]] static auto bold(Options x) -> Divider;

    /// Twin Divider: ═║
    [[nodiscard]] static auto twin(std::string label = "") -> Divider;
    [[nodiscard]] static auto twin(Options x) -> Divider;

    /// Dashed Divider: ╌╎
    [[nodiscard]] static auto dashed(std::string label = "") -> Divider;
    [[nodiscard]] static auto dashed(Options x) -> Divider;

    /// Dotted Divider: ┄┆
    [[nodiscard]] static auto dotted(std::string label = "") -> Divider;
    [[nodiscard]] static auto dotted(Options x) -> Divider;

    /// ASCII Divider: -|
    [[nodiscard]] static auto ascii(std::string label = "") -> Divider;
    [[nodiscard]] static auto ascii(Options x) -> Divider;

    [[nodiscard]] static auto empty(std::string label = "") -> Divider;
    [[nodiscard]] static auto empty(Options x) -> Divider;
};

}  // namespace ox
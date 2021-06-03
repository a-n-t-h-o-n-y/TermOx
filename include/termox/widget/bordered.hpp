#ifndef TERMOX_WIDGET_BORDER_DEV_HPP
#define TERMOX_WIDGET_BORDER_DEV_HPP
#include <limits>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

#include <termox/painter/color.hpp>
#include <termox/widget/focus_policy.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/tuple.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widgets/tile.hpp>

namespace ox {

/// Defines which border walls are enabled and how they are displayed.
/** Corners are only enabled if the two adjoining walls are enabled. */
struct Border {
    using Wall = std::optional<Glyph>;

    Wall north;
    Wall south;
    Wall east;
    Wall west;
    Glyph nw_corner;  // North West
    Glyph ne_corner;  // North East
    Glyph sw_corner;  // South West
    Glyph se_corner;  // South East
};

}  // namespace ox

namespace ox::detail {

/// Combines each Wall/Glyph of \p b with pipe operator and \p x.
template <typename T>
auto constexpr pipe_all(Border& b, T x) -> Border&
{
    if (b.north.has_value())
        *b.north |= x;
    if (b.south.has_value())
        *b.south |= x;
    if (b.east.has_value())
        *b.east |= x;
    if (b.west.has_value())
        *b.west |= x;
    b.nw_corner |= x;
    b.ne_corner |= x;
    b.sw_corner |= x;
    b.se_corner |= x;
    return b;
}

}  // namespace ox::detail
namespace ox {

[[nodiscard]] auto constexpr operator|(Border b, Background_color bg) -> Border
{
    return detail::pipe_all(b, bg);
}

auto constexpr operator|=(Border& b, Background_color bg) -> Border&
{
    return detail::pipe_all(b, bg);
}

[[nodiscard]] auto constexpr operator|(Border b, Foreground_color fg) -> Border
{
    return detail::pipe_all(b, fg);
}

auto constexpr operator|=(Border& b, Foreground_color fg) -> Border&
{
    return detail::pipe_all(b, fg);
}

[[nodiscard]] auto constexpr operator|(Border b, Traits t) -> Border
{
    return detail::pipe_all(b, t);
}

auto constexpr operator|=(Border& b, Traits t) -> Border&
{
    return detail::pipe_all(b, t);
}

/// Disable the North Wall.
[[nodiscard]] auto drop_north(Border b) -> Border;

/// Disable the South Wall.
[[nodiscard]] auto drop_south(Border b) -> Border;

/// Disable the East Wall.
[[nodiscard]] auto drop_east(Border b) -> Border;

/// Disable the West Wall.
[[nodiscard]] auto drop_west(Border b) -> Border;

/// Disable everything except the North Wall.
[[nodiscard]] auto take_north(Border b) -> Border;

/// Disable everything except the South Wall.
[[nodiscard]] auto take_south(Border b) -> Border;

/// Disable everything except the East Wall.
[[nodiscard]] auto take_east(Border b) -> Border;

/// Disable everything except the West Wall.
[[nodiscard]] auto take_west(Border b) -> Border;

}  // namespace ox

namespace ox::border {

[[nodiscard]] auto constexpr none() -> Border
{
    return {std::nullopt, std::nullopt, std::nullopt, std::nullopt,
            U' ',         U' ',         U' ',         U' '};
};

[[nodiscard]] auto constexpr squared() -> Border
{
    return {U'─', U'─', U'│', U'│', U'┌', U'┐', U'└', U'┘'};
};

[[nodiscard]] auto constexpr rounded() -> Border
{
    return {U'─', U'─', U'│', U'│', U'╭', U'╮', U'╰', U'╯'};
};

[[nodiscard]] auto constexpr plus_corners() -> Border
{
    return {U'─', U'─', U'│', U'│', U'+', U'+', U'+', U'+'};
};

[[nodiscard]] auto constexpr asterisk() -> Border
{
    return {U'*', U'*', U'*', U'*', U'*', U'*', U'*', U'*'};
};

[[nodiscard]] auto constexpr doubled() -> Border
{
    return {U'═', U'═', U'║', U'║', U'╔', U'╗', U'╚', U'╝'};
};

[[nodiscard]] auto constexpr bold() -> Border
{
    return {U'━', U'━', U'┃', U'┃', U'┏', U'┓', U'┗', U'┛'};
};

[[nodiscard]] auto constexpr dashed_1() -> Border
{
    return {U'╶', U'╶', U'╷', U'╷', U'┌', U'┐', U'└', U'┘'};
};

[[nodiscard]] auto constexpr bold_dashed_1() -> Border
{
    return {U'╺', U'╺', U'╻', U'╻', U'┏', U'┓', U'┗', U'┛'};
};

[[nodiscard]] auto constexpr dashed_2() -> Border
{
    return {U'╌', U'╌', U'╎', U'╎', U'┌', U'┐', U'└', U'┘'};
};

[[nodiscard]] auto constexpr bold_dashed_2() -> Border
{
    return {U'╍', U'╍', U'╏', U'╏', U'┏', U'┓', U'┗', U'┛'};
};

[[nodiscard]] auto constexpr dashed_3() -> Border
{
    return {U'┄', U'┄', U'┆', U'┆', U'┌', U'┐', U'└', U'┘'};
};

[[nodiscard]] auto constexpr bold_dashed_3() -> Border
{
    return {U'┅', U'┅', U'┇', U'┇', U'┏', U'┓', U'┗', U'┛'};
};

[[nodiscard]] auto constexpr dashed_4() -> Border
{
    return {U'┈', U'┈', U'┊', U'┊', U'┌', U'┐', U'└', U'┘'};
};

[[nodiscard]] auto constexpr bold_dashed_4() -> Border
{
    return {U'┉', U'┉', U'┋', U'┋', U'┏', U'┓', U'┗', U'┛'};
};

[[nodiscard]] auto constexpr block_1() -> Border
{
    return {U'█', U'█', U'█', U'█', U'█', U'█', U'█', U'█'};
};

[[nodiscard]] auto constexpr block_2() -> Border
{
    return {U'▓', U'▓', U'▓', U'▓', U'▓', U'▓', U'▓', U'▓'};
};

[[nodiscard]] auto constexpr block_3() -> Border
{
    return {U'▒', U'▒', U'▒', U'▒', U'▒', U'▒', U'▒', U'▒'};
};

[[nodiscard]] auto constexpr block_4() -> Border
{
    return {U'░', U'░', U'░', U'░', U'░', U'░', U'░', U'░'};
};

[[nodiscard]] auto constexpr half_block() -> Border
{
    return {U'▄' | Trait::Inverse,
            U'▄',
            U'▌' | Trait::Inverse,
            U'▌',
            U'▛',
            U'▜',
            U'▙',
            U'▟'};
};

[[nodiscard]] auto constexpr half_block_inner_1() -> Border
{
    return {U'▄', U'▄' | Trait::Inverse,
            U'▌', U'▌' | Trait::Inverse,
            U'▗', U'▖',
            U'▝', U'▘'};
};

[[nodiscard]] auto constexpr half_block_inner_2() -> Border
{
    return {U'▄', U'▄' | Trait::Inverse,
            U'▌', U'▌' | Trait::Inverse,
            U'▚', U'▞',
            U'▞', U'▚'};
};

[[nodiscard]] auto constexpr block_corners() -> Border
{
    return {U'─', U'─', U'│', U'│', U'▘', U'▝', U'▖', U'▗'};
};

[[nodiscard]] auto constexpr floating_block_corners() -> Border
{
    return {U'─', U'─', U'│', U'│', U'▗', U'▖', U'▝', U'▘'};
};

}  // namespace ox::border

namespace ox {

/// Creates a Bordered widget, which wraps the template type in a Border.
/** The wrapped widget is accessible from the `wrapped` member. */
template <typename Widget_t, typename Column = layout::Vertical<>>
class Bordered : public layout::Horizontal<Column> {
   private:
    /// Border Corner Widget
    using Corner = Tile;

    /// Border Wall Widget
    template <template <typename> typename Layout_t>
    class Wall : public Widget {
       public:
        Wall(Glyph g)
        {
            if constexpr (layout::is_vertical_v<Layout_t<Widget>>)
                this->width_policy = Size_policy::fixed(1);
            else
                this->height_policy = Size_policy::fixed(1);
            this->set_glyph(g);
        };

       public:
        /// Set the single Glyph that is repeated for length of Wall.
        void set_glyph(Glyph g) { this->set_wallpaper(g); }

        /// Return the currently set Glyph.
        [[nodiscard]] auto glyph() const -> Glyph
        {
            return this->get_wallpaper();
        }
    };

    using HWall = Wall<layout::Horizontal>;
    using VWall = Wall<layout::Vertical>;

   public:
    struct Parameters {
        Border border;
        typename Widget_t::Parameters wrapped_parameters;
    };

   public:
    Widget_t& wrapped;

    /// Called on set_border(...) after the Border has been set.
    sl::Signal<void()> border_set;

   public:
    template <typename... Args>
    explicit Bordered(Border b, Args&&... wrapped_args)
        : wrapped{this->template make_child<Column>()
                      .template make_child<Widget_t>(
                          std::forward<Args>(wrapped_args)...)},
          border_{std::move(b)}
    {
        this->initialize();
    }

    // This overload is required for apple-clang and clang 9 & 10.
    // Otherwise you'd just have Border have a default value above.
    Bordered()
        : wrapped{this->template make_child<Column>()
                      .template make_child<Widget_t>()},
          border_{border::rounded()}
    {
        this->initialize();
    }

    explicit Bordered(Parameters p)
        : Bordered{std::move(p.border), std::move(p.wrapped_parameters)}
    {}

    /// Create a border around an existing Widget.
    /** Defaults to border::rounded(), can be modified with pipe op. */
    explicit Bordered(std::unique_ptr<Widget_t> w_ptr)
        : wrapped{this->template make_child<Column>().append_child(
              std::move(w_ptr))},
          border_{border::rounded()}
    {
        static_assert(std::is_base_of<Widget, Widget_t>::value,
                      "Bordered: Widget_t must be a Widget type");
        this->initialize();
    }

   public:
    /// Set the currently used Border, updating the display.
    void set_border(Border b)
    {
        this->delete_all_border_pieces(border_);
        border_ = b;
        this->build_border_pieces(border_);
        border_set.emit();
    }

    /// Return the currenly set Border.
    auto border() const -> Border { return border_; }

   private:
    Border border_;

   private:
    /// Creates new border piece Widgets using \p s as a guide.
    void build_border_pieces(Border const& b)
    {
        auto const left_column =
            b.west.has_value() ? std::optional{0} : std::nullopt;
        auto const mid_column = b.west.has_value() ? 1 : 0;
        auto const right_column =
            b.east.has_value() ? std::optional{mid_column + 1} : std::nullopt;

        auto const fp   = wrapped.focus_policy;
        auto const init = [&](auto& w) {
            w.focus_policy = fp;
            w.focused_in.connect([&] { System::set_focus(wrapped); });
        };

        // Call order is important.
        if (left_column.has_value()) {
            auto& lc =
                this->insert_child(std::make_unique<Column>(), *left_column);
            lc.width_policy = Size_policy::fixed(1);
            if (b.north.has_value())
                init(lc.template make_child<Corner>(b.nw_corner));
            init(lc.template make_child<VWall>(*b.west));
            if (b.south.has_value())
                init(lc.template make_child<Corner>(b.sw_corner));
        }
        if (right_column.has_value()) {
            auto& rc =
                this->insert_child(std::make_unique<Column>(), *right_column);
            rc.width_policy = Size_policy::fixed(1);
            if (b.north.has_value())
                init(rc.template make_child<Corner>(b.ne_corner));
            init(rc.template make_child<VWall>(*b.east));
            if (b.south.has_value())
                init(rc.template make_child<Corner>(b.se_corner));
        }
        if (b.north.has_value()) {
            init(this->get_children()[mid_column].insert_child(
                std::make_unique<HWall>(*b.north), 0));
        }
        if (b.south.has_value()) {
            init(this->get_children()[mid_column].template make_child<HWall>(
                *b.south));
        }
    }

    /// Using \p s as a guide, will delete all border related Widgets.
    void delete_all_border_pieces(Border b)
    {
        auto const left_column =
            b.west.has_value() ? std::optional{0} : std::nullopt;
        auto const mid_column = b.west.has_value() ? 1 : 0;
        auto const right_column =
            b.east.has_value() ? std::optional{mid_column + 1} : std::nullopt;

        // Call order is important.
        if (b.south.has_value()) {
            auto const back = b.north.has_value() ? 2 : 1;
            this->get_children()[mid_column].remove_and_delete_child_at(back);
        }
        if (b.north.has_value())
            this->get_children()[mid_column].remove_and_delete_child_at(0);
        if (right_column.has_value())
            this->remove_and_delete_child_at(*right_column);
        if (left_column.has_value())
            this->remove_and_delete_child_at(*left_column);
    }

    void initialize()
    {
        // Can't use pipe:: in this file.
        this->focus_policy = wrapped.focus_policy;
        this->focused_in.connect([&] { System::set_focus(wrapped); });

        this->build_border_pieces(border_);
    }
};

/// Helper function to create an instance of Bordered<Widget_t>.
template <typename Widget_t>
[[nodiscard]] auto bordered(typename Bordered<Widget_t>::Parameters p = {
                                border::rounded(),
                                {}}) -> std::unique_ptr<Bordered<Widget_t>>
{
    return std::make_unique<Bordered<Widget_t>>(std::move(p));
}

/// Helper function to create an instance of Bordered<Widget_t>.
template <typename Widget_t, typename... Args>
[[nodiscard]] auto bordered(Border b = border::rounded(),
                            Args&&... wrapped_args)
{
    return std::make_unique<Bordered<Widget_t>>(
        std::move(b), std::forward<Args>(wrapped_args)...);
}

template <typename Widget_t>
[[nodiscard]] auto bordered(std::unique_ptr<Widget_t> w_ptr)
    -> std::unique_ptr<Bordered<Widget_t>>
{
    return std::make_unique<Bordered<Widget_t>>(std::move(w_ptr));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_BORDER_DEV_HPP

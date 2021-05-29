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

namespace ox::border {

struct Segments {
    using Segment = std::optional<Glyph>;

    Segment north;
    Segment south;
    Segment east;
    Segment west;
    Glyph nw_corner;
    Glyph ne_corner;
    Glyph sw_corner;
    Glyph se_corner;
};

namespace detail {

/// Combines each segment of \p s with pipe operator and \p x.
template <typename T>
auto constexpr pipe_all(Segments& s, T x) -> Segments&
{
    if (s.north.has_value())
        *s.north |= x;
    if (s.south.has_value())
        *s.south |= x;
    if (s.east.has_value())
        *s.east |= x;
    if (s.west.has_value())
        *s.west |= x;
    s.nw_corner |= x;
    s.ne_corner |= x;
    s.sw_corner |= x;
    s.se_corner |= x;
    return s;
}

}  // namespace detail

[[nodiscard]] auto constexpr operator|(Segments s, Background_color bg)
    -> Segments
{
    return detail::pipe_all(s, bg);
}

auto constexpr operator|=(Segments& s, Background_color bg) -> Segments&
{
    return detail::pipe_all(s, bg);
}

[[nodiscard]] auto constexpr operator|(Segments s, Foreground_color fg)
    -> Segments
{
    return detail::pipe_all(s, fg);
}

auto constexpr operator|=(Segments& s, Foreground_color fg) -> Segments&
{
    return detail::pipe_all(s, fg);
}

[[nodiscard]] auto constexpr operator|(Segments s, Traits t) -> Segments
{
    return detail::pipe_all(s, t);
}

auto constexpr operator|=(Segments& s, Traits t) -> Segments&
{
    return detail::pipe_all(s, t);
}

[[nodiscard]] auto constexpr none() -> Segments
{
    return {std::nullopt, std::nullopt, std::nullopt, std::nullopt,
            U' ',         U' ',         U' ',         U' '};
};

[[nodiscard]] auto constexpr squared() -> Segments
{
    return {U'─', U'─', U'│', U'│', U'┌', U'┐', U'└', U'┘'};
};

[[nodiscard]] auto constexpr rounded() -> Segments
{
    return {U'─', U'─', U'│', U'│', U'╭', U'╮', U'╰', U'╯'};
};

[[nodiscard]] auto constexpr plus_corners() -> Segments
{
    return {U'─', U'─', U'│', U'│', U'+', U'+', U'+', U'+'};
};

[[nodiscard]] auto constexpr asterisk() -> Segments
{
    return {U'*', U'*', U'*', U'*', U'*', U'*', U'*', U'*'};
};

[[nodiscard]] auto constexpr doubled() -> Segments
{
    return {U'═', U'═', U'║', U'║', U'╔', U'╗', U'╚', U'╝'};
};

[[nodiscard]] auto constexpr bold() -> Segments
{
    return {U'━', U'━', U'┃', U'┃', U'┏', U'┓', U'┗', U'┛'};
};

[[nodiscard]] auto constexpr dashed_1() -> Segments
{
    return {U'╶', U'╶', U'╷', U'╷', U'┌', U'┐', U'└', U'┘'};
};

[[nodiscard]] auto constexpr bold_dashed_1() -> Segments
{
    return {U'╺', U'╺', U'╻', U'╻', U'┏', U'┓', U'┗', U'┛'};
};

[[nodiscard]] auto constexpr dashed_2() -> Segments
{
    return {U'╌', U'╌', U'╎', U'╎', U'┌', U'┐', U'└', U'┘'};
};

[[nodiscard]] auto constexpr bold_dashed_2() -> Segments
{
    return {U'╍', U'╍', U'╏', U'╏', U'┏', U'┓', U'┗', U'┛'};
};

[[nodiscard]] auto constexpr dashed_3() -> Segments
{
    return {U'┄', U'┄', U'┆', U'┆', U'┌', U'┐', U'└', U'┘'};
};

[[nodiscard]] auto constexpr bold_dashed_3() -> Segments
{
    return {U'┅', U'┅', U'┇', U'┇', U'┏', U'┓', U'┗', U'┛'};
};

[[nodiscard]] auto constexpr dashed_4() -> Segments
{
    return {U'┈', U'┈', U'┊', U'┊', U'┌', U'┐', U'└', U'┘'};
};

[[nodiscard]] auto constexpr bold_dashed_4() -> Segments
{
    return {U'┉', U'┉', U'┋', U'┋', U'┏', U'┓', U'┗', U'┛'};
};

[[nodiscard]] auto constexpr block_1() -> Segments
{
    return {U'█', U'█', U'█', U'█', U'█', U'█', U'█', U'█'};
};

[[nodiscard]] auto constexpr block_2() -> Segments
{
    return {U'▓', U'▓', U'▓', U'▓', U'▓', U'▓', U'▓', U'▓'};
};

[[nodiscard]] auto constexpr block_3() -> Segments
{
    return {U'▒', U'▒', U'▒', U'▒', U'▒', U'▒', U'▒', U'▒'};
};

[[nodiscard]] auto constexpr block_4() -> Segments
{
    return {U'░', U'░', U'░', U'░', U'░', U'░', U'░', U'░'};
};

[[nodiscard]] auto constexpr half_block() -> Segments
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

[[nodiscard]] auto constexpr half_block_inner_1() -> Segments
{
    return {U'▄', U'▄' | Trait::Inverse,
            U'▌', U'▌' | Trait::Inverse,
            U'▗', U'▖',
            U'▝', U'▘'};
};

[[nodiscard]] auto constexpr half_block_inner_2() -> Segments
{
    return {U'▄', U'▄' | Trait::Inverse,
            U'▌', U'▌' | Trait::Inverse,
            U'▚', U'▞',
            U'▞', U'▚'};
};

[[nodiscard]] auto constexpr block_corners() -> Segments
{
    return {U'─', U'─', U'│', U'│', U'▘', U'▝', U'▖', U'▗'};
};

[[nodiscard]] auto constexpr floating_block_corners() -> Segments
{
    return {U'─', U'─', U'│', U'│', U'▗', U'▖', U'▝', U'▘'};
};

[[nodiscard]] inline auto drop_north(Segments s) -> Segments
{
    s.north = std::nullopt;
    return s;
}

[[nodiscard]] inline auto drop_south(Segments s) -> Segments
{
    s.south = std::nullopt;
    return s;
}

[[nodiscard]] inline auto drop_east(Segments s) -> Segments
{
    s.east = std::nullopt;
    return s;
}

[[nodiscard]] inline auto drop_west(Segments s) -> Segments
{
    s.west = std::nullopt;
    return s;
}

[[nodiscard]] inline auto take_north(Segments s) -> Segments
{
    s.south = std::nullopt;
    s.east  = std::nullopt;
    s.west  = std::nullopt;
    return s;
}

[[nodiscard]] inline auto take_south(Segments s) -> Segments
{
    s.north = std::nullopt;
    s.east  = std::nullopt;
    s.west  = std::nullopt;
    return s;
}

[[nodiscard]] inline auto take_east(Segments s) -> Segments
{
    s.north = std::nullopt;
    s.south = std::nullopt;
    s.west  = std::nullopt;
    return s;
}

[[nodiscard]] inline auto take_west(Segments s) -> Segments
{
    s.north = std::nullopt;
    s.south = std::nullopt;
    s.east  = std::nullopt;
    return s;
}

}  // namespace ox::border

namespace ox {

template <typename Widget_t, typename Column = layout::Vertical<>>
class Bordered : public layout::Horizontal<Column> {
    /// Border Corner Widget
    using Corner = Tile;

    /// Border Wall Widget
    template <template <typename> typename Layout_t>
    class Wall : public Widget {
       public:
        Wall(Glyph g)
        {
            if constexpr (layout::is_vertical_v<Layout_t<Widget>>)
                this->width_policy.fixed(1);
            else
                this->height_policy.fixed(1);
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
        border::Segments segments;
        typename Widget_t::Parameters wrapped_parameters;
    };

   public:
    Widget_t& wrapped;

   public:
    template <typename... Args>
    explicit Bordered(border::Segments s, Args&&... wrapped_args)
        : wrapped{this->template make_child<Column>()
                      .template make_child<Widget_t>(
                          std::forward<Args>(wrapped_args)...)},
          segments_{std::move(s)}
    {
        this->initialize();
    }

    // This overload is required for apple-clang and clang 9 & 10.
    // Otherwise you'd just have Segments have a default value above.
    Bordered()
        : wrapped{this->template make_child<Column>()
                      .template make_child<Widget_t>()},
          segments_{border::rounded()}
    {
        this->initialize();
    }

    explicit Bordered(Parameters p)
        : Bordered{std::move(p.segments), std::move(p.wrapped_parameters)}
    {}

    /// Create a border around an existing Widget.
    /** Defaults to border::rounded(), can be modified with pipe op. */
    explicit Bordered(std::unique_ptr<Widget_t> w_ptr)
        : wrapped{this->template make_child<Column>().append_child(
              std::move(w_ptr))},
          segments_{border::rounded()}
    {
        static_assert(std::is_base_of<Widget, Widget_t>::value,
                      "Bordered: Widget_t must be a Widget type");
        this->initialize();
    }

   public:
    /// Set the currently used Segments, updating the display.
    void set(border::Segments x)
    {
        this->delete_all_border_pieces(segments_);

        segments_ = x;

        this->build_border_pieces(segments_);
    }

    /// Return the currenly set Segments.
    auto segments() const -> border::Segments { return segments_; }

   private:
    border::Segments segments_;

   private:
    /// Creates new border piece Widgets using \p s as a guide.
    void build_border_pieces(border::Segments const& s)
    {
        auto const left_column =
            s.west.has_value() ? std::optional{0} : std::nullopt;
        auto const mid_column = s.west.has_value() ? 1 : 0;
        auto const right_column =
            s.east.has_value() ? std::optional{mid_column + 1} : std::nullopt;

        // Call order is important.
        if (left_column.has_value()) {
            auto& lc =
                this->insert_child(std::make_unique<Column>(), *left_column);
            lc.width_policy.fixed(1);
            if (s.north.has_value())
                lc.template make_child<Corner>(s.nw_corner);
            lc.template make_child<VWall>(*s.west);
            if (s.south.has_value())
                lc.template make_child<Corner>(s.sw_corner);
        }
        if (right_column.has_value()) {
            auto& rc =
                this->insert_child(std::make_unique<Column>(), *right_column);
            rc.width_policy.fixed(1);
            if (s.north.has_value())
                rc.template make_child<Corner>(s.ne_corner);
            rc.template make_child<VWall>(*s.east);
            if (s.south.has_value())
                rc.template make_child<Corner>(s.se_corner);
        }
        if (s.north.has_value()) {
            this->get_children()[mid_column].insert_child(
                std::make_unique<HWall>(*s.north), 0);
        }
        if (s.south.has_value()) {
            this->get_children()[mid_column].template make_child<HWall>(
                *s.south);
        }
    }

    /// Using \p s as a guide, will delete all border related Widgets.
    void delete_all_border_pieces(border::Segments s)
    {
        auto const left_column =
            s.west.has_value() ? std::optional{0} : std::nullopt;
        auto const mid_column = s.west.has_value() ? 1 : 0;
        auto const right_column =
            s.east.has_value() ? std::optional{mid_column + 1} : std::nullopt;

        // Call order is important.
        if (s.south.has_value()) {
            auto const back = s.north.has_value() ? 2 : 1;
            this->get_children()[mid_column].remove_and_delete_child_at(back);
        }
        if (s.north.has_value())
            this->get_children()[mid_column].remove_and_delete_child_at(0);
        if (right_column.has_value())
            this->remove_and_delete_child_at(*right_column);
        if (left_column.has_value())
            this->remove_and_delete_child_at(*left_column);
    }

    void initialize()
    {
        // Can't use pipe:: in this file.
        this->focus_policy = Focus_policy::Strong;
        this->focused_in.connect([&] { System::set_focus(wrapped); });

        this->build_border_pieces(segments_);
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
[[nodiscard]] auto bordered(border::Segments s = border::rounded(),
                            Args&&... wrapped_args)
{
    return std::make_unique<Bordered<Widget_t>>(
        std::move(s), std::forward<Args>(wrapped_args)...);
}

template <typename Widget_t>
[[nodiscard]] auto bordered(std::unique_ptr<Widget_t> w_ptr)
    -> std::unique_ptr<Bordered<Widget_t>>
{
    return std::make_unique<Bordered<Widget_t>>(std::move(w_ptr));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_BORDER_DEV_HPP

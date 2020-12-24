#include <termox/painter/detail/screen.hpp>

#include <iterator>
#include <mutex>

#include <termox/painter/brush.hpp>
#include <termox/painter/color.hpp>
#include <termox/painter/detail/find_empty_space.hpp>
#include <termox/painter/detail/is_paintable.hpp>
#include <termox/painter/detail/screen_descriptor.hpp>
#include <termox/painter/detail/screen_mask.hpp>
#include <termox/painter/detail/staged_changes.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/trait.hpp>
#include <termox/system/detail/focus.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/output.hpp>
#include <termox/terminal/terminal.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

namespace {
using namespace ox;

template <typename Map_t>
auto contains(typename Map_t::key_type const& value, Map_t const& map) -> bool
{
    return map.count(value) > 0;
}

auto has_children(Widget const& widg) -> bool
{
    return !(widg.get_children().empty());
}

/// Covers space unowned by any child widget with wallpaper.
void paint_unowned_tiles(Widget const& layout, Glyph wallpaper)
{
    auto const empty_space = detail::find_empty_space(layout);
    auto const y_begin     = empty_space.offset().y;
    auto const x_begin     = empty_space.offset().x;
    auto const y_end       = y_begin + empty_space.area().height;
    auto const x_end       = x_begin + empty_space.area().width;
    for (auto y = y_begin; y < y_end; ++y) {
        for (auto x = x_begin; x < x_end; ++x) {
            if (empty_space.at(x, y))
                output::put(x, y, wallpaper);
        }
    }
}

// Paint every point of \p widg with wallpaper or from \p staged_tiles.
void paint_to_terminal(Widget& widg,
                       detail::Screen_descriptor const& staged_tiles)
{
    auto const wallpaper          = widg.generate_wallpaper();
    auto const [x_begin, y_begin] = std::pair{widg.x(), widg.y()};
    auto const [x_end, y_end] =
        std::pair{x_begin + widg.outer_width(), y_begin + widg.outer_height()};
    auto const is_layout = has_children(widg);
    if (is_layout)
        paint_unowned_tiles(widg, wallpaper);
    for (auto y = y_begin; y < y_end; ++y) {
        for (auto x = x_begin; x < x_end; ++x) {
            if (contains({x, y}, staged_tiles)) {
                auto tile  = staged_tiles.at({x, y});
                tile.brush = merge(tile.brush, widg.brush);
                output::put(x, y, tile);
            }
            else if (not is_layout)
                output::put(x, y, wallpaper);
        }
    }
}

void set_cursor(Point offset, Cursor const& cursor)
{
    System::terminal.show_cursor();
    auto const x_global = offset.x + cursor.x();
    auto const y_global = offset.y + cursor.y();
    output::move_cursor(x_global, y_global);
}

auto cursor_is_within_area(Cursor const& cursor, Area const& a) -> bool
{
    return cursor.x() < a.width && cursor.y() < a.height;
}

auto has_valid_cursor(Widget const* w) -> bool
{
    if (w == nullptr)
        return false;
    return w->cursor.enabled() && detail::is_paintable(*w) &&
           cursor_is_within_area(w->cursor, w->area());
}

void remove_cursor() { System::terminal.show_cursor(false); }

}  // namespace

namespace ox::detail {

void Screen::flush(Staged_changes::Map_t const& changes)
{
    auto refresh = false;
    for (auto const& widg_description : changes) {
        auto& widget = *widg_description.first;
        if (is_paintable(widget)) {
            paint_to_terminal(widget, widg_description.second);
            refresh = true;
        }
        else
            widget.screen_state().clear();
    }
    if (refresh)
        output::refresh();
}

void Screen::display_cursor()
{
    auto const* focus = detail::Focus::focus_widget();
    if (has_valid_cursor(focus))
        set_cursor(focus->inner_top_left(), focus->cursor);
    else
        remove_cursor();
}

}  // namespace ox::detail

#include <cppurses/painter/detail/screen.hpp>

#include <iterator>
#include <mutex>

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/find_empty_space.hpp>
#include <cppurses/painter/detail/is_paintable.hpp>
#include <cppurses/painter/detail/screen_descriptor.hpp>
#include <cppurses/painter/detail/screen_mask.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/trait.hpp>
#include <cppurses/system/detail/focus.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/terminal/output.hpp>
#include <cppurses/terminal/terminal.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;

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
void paint_unowned_tiles(Widget const& layout, Glyph const& wallpaper)
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
                auto tile = staged_tiles.at({x, y});
                imprint(widg.brush, tile.brush);
                output::put(x, y, tile);
            }
            else if (not is_layout)
                output::put(x, y, wallpaper);
        }
    }
}

}  // namespace

namespace cppurses::detail {

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

void Screen::set_cursor_on_focus_widget()
{
    auto const* focus = detail::Focus::focus_widget();
    if (focus != nullptr and focus->cursor.enabled() and is_paintable(*focus)) {
        System::terminal.show_cursor();
        auto const x_global = focus->inner_x() + focus->cursor.x();
        auto const y_global = focus->inner_y() + focus->cursor.y();
        output::move_cursor(x_global, y_global);
    }
    else
        System::terminal.show_cursor(false);
}

}  // namespace cppurses::detail

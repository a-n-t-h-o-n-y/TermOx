#include <cppurses/painter/detail/flush.hpp>

#include <mutex>
#include <vector>

#include <cppurses/painter/detail/add_default_attributes.hpp>
#include <cppurses/painter/detail/is_not_paintable.hpp>
#include <cppurses/painter/detail/screen_state.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
namespace detail {

void flush(const Staged_changes& changes, bool is_background) {
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock{mtx};

    for (const auto& pair : changes) {
        Widget* widg{pair.first};
        const Screen_descriptor& changes_map{pair.second};
        if (detail::is_not_paintable(widg)) {
            continue;
        }
        // BACKGROUNDS COVER LEFTOVERS
        std::vector<Point> to_delete;
        for (const auto& point_tile : widg->screen_state().tiles) {
            const Point& point{point_tile.first};
            if (changes_map.count(point) == 0) {
                Glyph background{widg->find_background_tile()};
                System::paint_engine().put(point.x, point.y, background);
                to_delete.push_back(point);
            }
        }
        for (const Point& p : to_delete) {
            widg->screen_state().tiles.erase(p);
        }
        // NEW TILES PRINTED TO SCREEN
        for (const auto& point_tile : changes_map) {
            const Point& point{point_tile.first};
            Glyph tile{point_tile.second};
            if (!is_background) {
                detail::add_default_attributes(tile, widg->brush);
            }
            auto screen_iter = widg->screen_state().tiles.find(point);
            if (screen_iter != std::end(widg->screen_state().tiles) &&
                screen_iter->second == tile) {
                continue;
            } else {
                System::paint_engine().put(point.x, point.y, tile);
                widg->screen_state().tiles[point] = tile;
            }
        }
        if (is_background) {
            widg->screen_state().tiles.clear();
        }
    }
    System::paint_engine().refresh();
}
// after call to flush(), clear the contents of changes object.

}  // namespace detail
}  // namespace cppurses

#include <cppurses/painter/painter.hpp>

#include <cstddef>
#include <cstring>

#include <optional/optional.hpp>

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/is_not_paintable.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/paint_buffer.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

#define DEBUG_PAINTER_PUT_GLOBAL

#if defined(DEBUG_PAINTER_PUT_GLOBAL)
#include <fstream>
#endif

namespace cppurses {

Painter::Painter(Widget* widg)
    : widget_{widg},
      staged_changes_{System::find_event_loop().staged_changes()[widg]} {}

Painter::Painter(Widget* widg, detail::Staged_changes& changes)
    : widget_{widg}, staged_changes_{changes[widg]} {}

void Painter::put(const Glyph& tile, std::size_t x, std::size_t y) {
    if (x >= widget_->width() || y >= widget_->height()) {
        return;
    }
    std::size_t glob_x = widget_->inner_x() + x;
    std::size_t glob_y = widget_->inner_y() + y;
    this->put_global(tile, glob_x, glob_y);
}

void Painter::put(const Glyph& tile, Point position) {
    this->put(tile, position.x, position.y);
}

void Painter::put(const Glyph_string& text, std::size_t x, std::size_t y) {
    if (detail::is_not_paintable(widget_)) {
        return;
    }
    for (const Glyph& g : text) {
        this->put(g, x++, y);
    }
}

void Painter::put(const Glyph_string& text, Point position) {
    this->put(text, position.x, position.y);
}

void Painter::border(const Border& b) {
    if (!b.enabled || !widget_->enabled() || widget_->outer_width() == 0 ||
        widget_->outer_height() == 0) {
        return;
    }

    // Disqualified borders
    bool west_disqualified{widget_->outer_width() == 1};
    bool east_disqualified{widget_->outer_width() <= 2};
    bool north_disqualified{widget_->outer_height() == 1};
    bool south_disqualified{widget_->outer_height() <= 2};

    // North Wall
    Point north_left{widget_->inner_x(), widget_->y()};
    Point north_right{north_left.x + widget_->width() - 1, north_left.y};

    // South Wall
    Point south_left{north_left.x, widget_->y() + widget_->outer_height() - 1};
    Point south_right{north_right.x, south_left.y};

    // West Wall
    Point west_top{widget_->x(), widget_->inner_y()};
    Point west_bottom{west_top.x, west_top.y + widget_->height() - 1};

    // East Wall
    Point east_top{west_top.x + widget_->outer_width() - 1, west_top.y};
    Point east_bottom{east_top.x, west_bottom.y};

    // Corners
    Point north_east{east_top.x, north_left.y};
    Point north_west{west_top.x, north_right.y};
    Point south_east{east_bottom.x, south_left.y};
    Point south_west{west_bottom.x, south_right.y};

    // North
    if (b.north_enabled && !north_disqualified) {
        this->line_global(b.north, north_left, north_right);
    }
    // South
    if (b.south_enabled && !south_disqualified) {
        this->line_global(b.south, south_left, south_right);
    }
    // West
    if (b.west_enabled && !west_disqualified) {
        this->line_global(b.west, west_top, west_bottom);
    }
    // East
    if (b.east_enabled && !east_disqualified) {
        this->line_global(b.east, east_top, east_bottom);
    }
    // North-West
    if (b.north_west_enabled && !north_disqualified && !west_disqualified) {
        this->put_global(b.north_west, north_west);
    }
    // North-East
    if (b.north_east_enabled && !north_disqualified && !east_disqualified) {
        this->put_global(b.north_east, north_east);
    }
    // South-West
    if (b.south_west_enabled && !south_disqualified && !west_disqualified) {
        this->put_global(b.south_west, south_west);
    }
    // South-East
    if (b.south_east_enabled && !south_disqualified && !east_disqualified) {
        this->put_global(b.south_east, south_east);
    }

    // Stop out of bounds drawing for special cases.
    if (widget_->height() == 1 && north_disqualified && south_disqualified) {
        return;
    }
    if (widget_->width() == 1 && west_disqualified && east_disqualified) {
        return;
    }

    // Corners - Special Case Glyph Change
    // North-West
    if (!b.north_west_enabled && !b.north_enabled && b.west_enabled) {
        this->put_global(b.west, north_west);
    } else if (!b.north_west_enabled && !b.west_enabled && b.north_enabled) {
        this->put_global(b.north, north_west);
    }
    // North-East
    if (!b.north_east_enabled && !b.north_enabled && b.east_enabled) {
        this->put_global(b.east, north_east);
    } else if (!b.north_east_enabled && !b.east_enabled && b.north_enabled) {
        this->put_global(b.north, north_east);
    }
    // South-West
    if (!b.south_west_enabled && !b.south_enabled && b.west_enabled) {
        this->put_global(b.west, south_west);
    } else if (!b.south_west_enabled && !b.west_enabled && b.south_enabled) {
        this->put_global(b.south, south_west);
    }
    // South-East
    if (!b.south_east_enabled && !b.south_enabled && b.east_enabled) {
        this->put_global(b.east, south_east);
    } else if (!b.south_east_enabled && !b.east_enabled && b.south_enabled) {
        this->put_global(b.south, south_east);
    }
}

void Painter::fill(const Glyph& tile,
                   std::size_t x,
                   std::size_t y,
                   std::size_t width,
                   std::size_t height) {
    if (width == 0) {
        return;
    }
    const std::size_t y_limit{y + height};
    for (; y < y_limit; ++y) {
        this->line(tile, x, y, width - 1, y);
    }
}

void Painter::fill(const Glyph& tile,
                   Point point,
                   std::size_t width,
                   std::size_t height) {
    this->fill(tile, point.x, point.y, width, height);
}

// Does not call down to line_global because this needs bounds checking.
void Painter::line(const Glyph& tile,
                   std::size_t x1,
                   std::size_t y1,
                   std::size_t x2,
                   std::size_t y2) {
    // Horizontal
    if (y1 == y2) {
        for (; x1 <= x2; ++x1) {
            this->put(tile, x1, y1);
        }
    }  // Vertical
    else if (x1 == x2) {
        for (; y1 <= y2; ++y1) {
            this->put(tile, x1, y1);
        }
    }
}

void Painter::line(const Glyph& tile,
                   const Point& point_1,
                   const Point& point_2) {
    this->line(tile, point_1.x, point_1.y, point_2.x, point_2.y);
}

// GLOBAL COORDINATES - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Painter::put_global(const Glyph& tile, std::size_t x, std::size_t y) {
#if defined(DEBUG_PAINTER_PUT_GLOBAL)
    std::size_t width{System::max_width()};
    std::size_t height{System::max_height()};
    if (x >= width || y >= height) {
        std::ofstream l{"painter_put_global_log.txt", std::ios::app};
        l << "widget: " << widget_->name() << '\n';
        l << "Painting at: (" << x << ", " << y << ")\n";
        l << "widget width: " << widget_->outer_width()
          << " height: " << widget_->outer_height() << std::endl;
        l << "widget x: " << widget_->x() << " y: " << widget_->y()
          << std::endl;
        if (widget_->parent() != nullptr) {
            l << "parent: " << widget_->parent()->name() << '\n';
            l << "parent width: " << widget_->parent()->outer_width()
              << " height: " << widget_->parent()->outer_height() << std::endl;
            l << "parent x: " << widget_->parent()->x();
            l << " y: " << widget_->parent()->y() << '\n';
            if (widget_->parent()->parent() != nullptr) {
                l << "\tparent's parent: "
                  << widget_->parent()->parent()->name() << '\n';
                l << "\tparent's parent width: "
                  << widget_->parent()->parent()->outer_width()
                  << " height: " << widget_->parent()->parent()->outer_height()
                  << std::endl;
                l << "\tparent's parent x: "
                  << widget_->parent()->parent()->x();
                l << " y: " << widget_->parent()->parent()->y() << '\n';
                l << "\tparent's parent inner_x: "
                  << widget_->parent()->parent()->inner_x();
                l << " inner_y: " << widget_->parent()->parent()->inner_y()
                  << '\n';
                if (widget_->parent()->parent()->parent() != nullptr) {
                    l << "\t\tparent's parent's parent: "
                      << widget_->parent()->parent()->parent()->name() << '\n';
                    l << "\t\tparent's parent's parent width: "
                      << widget_->parent()->parent()->parent()->outer_width()
                      << " height: "
                      << widget_->parent()->parent()->parent()->outer_height()
                      << std::endl;
                    l << "\t\tparent's parent's parent x: "
                      << widget_->parent()->parent()->parent()->x();
                    l << " y: " << widget_->parent()->parent()->parent()->y()
                      << '\n';
                    l << "\t\tparent's parent's parent inner_x: "
                      << widget_->parent()->parent()->parent()->inner_x();
                    l << " inner_y: "
                      << widget_->parent()->parent()->parent()->inner_y()
                      << '\n';
                }
            }
        }
        l << "Screen Boundaries. Width: " << width << " Height: " << height
          << '\n';
        l << "- - - - - - - - - - - - - -" << std::endl;
        return;
    }
#endif
    staged_changes_[Point{x, y}] = tile;
}

void Painter::put_global(const Glyph& tile, Point position) {
    this->put_global(tile, position.x, position.y);
}

void Painter::line_global(const Glyph& tile,
                          std::size_t x1,
                          std::size_t y1,
                          std::size_t x2,
                          std::size_t y2) {
    // Horizontal
    if (y1 == y2) {
        for (; x1 <= x2; ++x1) {
            put_global(tile, x1, y1);
        }
    }  // Vertical
    else if (x1 == x2) {
        for (; y1 <= y2; ++y1) {
            put_global(tile, x1, y1);
        }
    }
}

void Painter::line_global(const Glyph& tile,
                          const Point& point_1,
                          const Point& point_2) {
    line_global(tile, point_1.x, point_1.y, point_2.x, point_2.y);
}

}  // namespace cppurses

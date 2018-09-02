#ifndef CPPURSES_WIDGET_LAYOUT_HPP
#define CPPURSES_WIDGET_LAYOUT_HPP
#include <cstddef>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
struct Area;
struct Point;

/// Base class for Layouts. Calls update_geometry virtual function when needed.
class Layout : public Widget {
   protected:
    /// Implement this in subclasses to send resize and move events to children.
    virtual void update_geometry() = 0;

    bool move_event(Point new_position, Point old_position) override;
    bool resize_event(Area new_size, Area old_size) override;
    bool child_added_event(Widget* child) override;
    bool child_removed_event(Widget* child) override;
    bool child_polished_event(Widget* child) override;

    struct Dimensions {
        Widget* widget;
        std::size_t width;
        std::size_t height;
    };

    struct Dimensions_reference {
        Widget* widget;
        std::size_t* width;
        std::size_t* height;
    };
};

// Free Functions
void set_background(Layout& l, Color c);
void set_foreground(Layout& l, Color c);

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_LAYOUT_HPP

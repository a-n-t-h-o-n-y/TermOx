#ifndef WIDGET_LAYOUT_HPP
#define WIDGET_LAYOUT_HPP
#include <cstddef>

#include <cppurses/widget/area.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

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
    bool paint_event() override;

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
#endif  // WIDGET_LAYOUT_HPP

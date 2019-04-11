#ifndef CPPURSES_WIDGET_LAYOUT_HPP
#define CPPURSES_WIDGET_LAYOUT_HPP
#include <cstddef>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
struct Area;
struct Point;
namespace layout {

/// Provided as a uniform interface for arranging child Widgets.
/** The update_geometry() function will be called each time there is a reason to
 *  believe that the children Widgets of this Widget will want to change
 *  position and size. */
class Layout : public Widget {
   protected:
    /// Clients override this to post Resize and Move events to children.
    /** This will be called each time the children Widgets possibly need to be
     *  rearranged. Triggered by Move_event, Resize_event, Child_added_event,
     *  Child_removed_event, and Child_polished_event. */
    virtual void update_geometry() = 0;

    bool move_event(Point new_position, Point old_position) override {
        this->update_geometry();
        return Widget::move_event(new_position, old_position);
    }

    bool resize_event(Area new_size, Area old_size) override {
        this->update_geometry();
        return Widget::resize_event(new_size, old_size);
    }

    bool child_added_event(Widget& child) override {
        this->update_geometry();
        return Widget::child_added_event(child);
    }

    bool child_removed_event(Widget& child) override {
        this->update_geometry();
        return Widget::child_removed_event(child);
    }

    bool child_polished_event(Widget& child) override {
        this->update_geometry();
        return Widget::child_polished_event(child);
    }

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

}  // namespace layout
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_LAYOUT_HPP

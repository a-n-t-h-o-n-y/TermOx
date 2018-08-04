#ifndef WIDGET_LAYOUT_HPP
#define WIDGET_LAYOUT_HPP
#include <cstddef>

#include <cppurses/widget/area.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

// Base class for Layouts
class Layout : public Widget {
   public:
    Layout();

   protected:
    bool child_added_event(Widget* child) override;
    bool child_removed_event(Widget* child) override;
    bool child_polished_event(Widget* child) override;
    bool paint_event() override;
    virtual void update_geometry() = 0;
    bool too_small_{false};
    bool needs_update_{false};

    Point last_inner_position_{0, 0};
    Area last_inner_size_{0, 0};

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

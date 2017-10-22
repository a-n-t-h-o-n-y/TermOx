#ifndef WIDGET_LAYOUT_HPP
#define WIDGET_LAYOUT_HPP
#include <cppurses/widget/widget.hpp>

#include <cstddef>

namespace cppurses {

// Base class for Layouts
class Layout : public Widget {
   public:
    Layout();

   protected:
    bool paint_event() override;
    virtual void update_geometry() = 0;
    bool too_small_{false};

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

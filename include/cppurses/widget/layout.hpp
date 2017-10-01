#ifndef WIDGET_LAYOUT_HPP
#define WIDGET_LAYOUT_HPP
#include "widget/widget.hpp"

namespace cppurses {

// Base class for Layouts
class Layout : public Widget {
   public:
    Layout();

   protected:
    bool paint_event() override;
    virtual void update_geometry() = 0;
    bool too_small_{false};
};

}  // namespace cppurses
#endif  // WIDGET_LAYOUT_HPP

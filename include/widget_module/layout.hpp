#ifndef WIDGET_MODULE_LAYOUT_HPP
#define WIDGET_MODULE_LAYOUT_HPP

#include "widget_module/widget.hpp"

namespace twf {
class Paint_event;

// Base class for Layouts
class Layout : public Widget {
   public:
    Layout();

   protected:
    bool paint_event(const Paint_event& event) override;
    virtual void update_geometry() = 0;
    bool too_small_{false};
};

}  // namespace twf
#endif  // WIDGET_MODULE_LAYOUT_HPP

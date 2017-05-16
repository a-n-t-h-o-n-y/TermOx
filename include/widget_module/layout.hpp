#ifndef WIDGET_MODULE_LAYOUT_HPP
#define WIDGET_MODULE_LAYOUT_HPP

#include <aml/signals/slot.hpp>
#include "widget_module/widget.hpp"
#include "system_module/events/child_event.hpp"
#include "system_module/events/paint_event.hpp"

namespace twf {

// Base class for Layouts
class Layout : public Widget {
   public:
    Layout();

    // Slots
    sig::Slot<void()> update_layout;

   protected:
    bool paint_event(const Paint_event& event) override;
    bool child_event(const Child_event& event) override;
    virtual void update_geometry() = 0;
    bool too_small_{false};

   private:
    void initialize();
};

}  // namespace twf
#endif  // WIDGET_MODULE_LAYOUT_HPP

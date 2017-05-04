#ifndef LAYOUT_HPP
#define LAYOUT_HPP

#include "widget.hpp"

#include <system_module/events/child_event.hpp>
#include <system_module/events/paint_event.hpp>
#include <system_module/object.hpp>

#include <aml/signals/slot.hpp>

namespace twf {

// Base class for Layouts
class Layout : public Widget {
   public:
    Layout();

    // Slots
    sig::Slot<void()> update_layout;

   protected:
    bool paint_event(const Paint_event& event) override;
    virtual void update_geometry() = 0;
    void paint_too_small_warning();

   private:
    void initialize();
};

}  // namespace twf
#endif  // LAYOUT_HPP

#ifndef LAYOUT_HPP
#define LAYOUT_HPP

#include "widget.hpp"

#include <system_module/events/child_event.hpp>
#include <system_module/events/paint_event.hpp>
#include <system_module/object.hpp>

#include <aml/signals/slot.hpp>

namespace mcurses {

// Base class for Layouts
class Layout : public Widget {
   public:
    Layout();

    void update() override;

    // Slots
    Slot<void()> update_layout;  // this should call update()

   protected:
    void child_event(Child_event& event) override;
    virtual void update_geometry() = 0;

   private:
    void initialize();
};

}  // namespace mcurses
#endif  // LAYOUT_HPP

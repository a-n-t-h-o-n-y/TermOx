#ifndef WIDGET_WIDGETS_CHECKBOX_HPP
#define WIDGET_WIDGETS_CHECKBOX_HPP
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/widget/widget.hpp>
#include <cstddef>
#include <cstdint>
#include <signals/signals.hpp>

namespace cppurses {

class Checkbox : public Widget {
   public:
    explicit Checkbox(Glyph_string title = "", int padding = 3);

    void toggle();
    bool is_checked() const;
    Glyph_string title() const;

    // Signals
    sig::Signal<void()> checked;
    sig::Signal<void()> unchecked;
    sig::Signal<void()> toggled;

   protected:
    bool paint_event() override;
    bool mouse_press_event(Mouse_button button,
                           Point global,
                           Point local,
                           std::uint8_t device_id) override;

   private:
    Glyph empty_box_{L'☐'};
    Glyph checked_box_{L'☒'};
    bool is_checked_{false};
    Glyph_string title_;
    int padding_;
};

void check(Checkbox& cb);
void uncheck(Checkbox& cb);

namespace slot {

sig::Slot<void()> toggle(Checkbox& cb);
sig::Slot<void()> check(Checkbox& cb);
sig::Slot<void()> uncheck(Checkbox& cb);

}  // namespace slot
}  // namespace cppurses
#endif  // WIDGET_WIDGETS_CHECKBOX_HPP

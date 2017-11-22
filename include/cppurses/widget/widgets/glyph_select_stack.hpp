#ifndef CPPURSES_WIDGET_WIDGETS_GLYPH_SELECT_STACK_HPP
#define CPPURSES_WIDGET_WIDGETS_GLYPH_SELECT_STACK_HPP
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/widgets/widget_stack.hpp>

namespace cppurses {

class Glyph_select_stack : public Widget_stack {
   public:
    void set_symbols(Glyph_string symbols);
    void append_symbols(Glyph_string symbols);

    void set_page_percent(float percent);

    // Signals
    sig::Signal<void(Glyph)> glyph_selected;

   protected:
    bool mouse_press_event_filter(Event_handler* receiver,
                                  Mouse_button button,
                                  Point global,
                                  Point local,
                                  std::uint8_t device_id) override;

    bool resize_event(Area new_size, Area old_size) override;

   private:
    Glyph_string symbols_;
    float percent_{0.0};

    void update_stack();
};

namespace slot {

sig::Slot<void(float)> set_page_percent(Glyph_select_stack& gss);
sig::Slot<void()> set_page_percent(Glyph_select_stack& gss, float percent);

}  // namespace slot

}  // namespace cppurses

#endif  // CPPURSES_WIDGET_WIDGETS_GLYPH_SELECT_STACK_HPP

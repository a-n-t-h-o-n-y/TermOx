#ifndef CPPURSES_WIDGET_WIDGETS_CHECKBOX_HPP
#define CPPURSES_WIDGET_WIDGETS_CHECKBOX_HPP
#include <cstddef>
#include <cstdint>

#include <signals/signals.hpp>

#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

/// Checkbox Widget that is either checked or not checked.
/** Uses mouse left button click to toggle between states. */
class Checkbox : public Widget {
   public:
    /// Construct a Checkbox with \p label and \p padding between label and box.
    explicit Checkbox(Glyph_string label = "", int padding = 3);

    /// Change state to be unchecked if initially checked, checked otherwise.
    void toggle();

    /// Set the state to be checked.
    void check();

    /// Set the state to be unchecked.
    void uncheck();

    /// Return whether Checkbox is currently checked.
    bool is_checked() const;

    /// Return the current label.
    const Glyph_string& label() const;

    /// Set the Glyph used for a checked box.
    void set_check_look(const Glyph& symbol);

    /// Set the Glyph used for an unchecked box.
    void set_uncheck_look(const Glyph& symbol);

    /// Return the Glyph representing the checked state.
    const Glyph& check_look() const;

    /// Return the Glyph representing the unchecked state.
    const Glyph& uncheck_look() const;

    /// Emitted only when box becomes checked.
    sig::Signal<void()> checked;

    /// Emitted only when box becomes unchecked.
    sig::Signal<void()> unchecked;

    /// Emitted every time the box changes state.
    sig::Signal<void()> toggled;

   protected:
    bool paint_event() override;
    bool mouse_press_event(const Mouse::State& mouse) override;

   private:
    Glyph unchecked_box_{L'☐'};
    Glyph checked_box_{L'☒'};
    bool checked_{false};
    Glyph_string label_;
    int padding_;
};

namespace slot {

sig::Slot<void()> toggle(Checkbox& cb);
sig::Slot<void()> check(Checkbox& cb);
sig::Slot<void()> uncheck(Checkbox& cb);

}  // namespace slot
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_CHECKBOX_HPP

#ifndef CPPURSES_WIDGET_WIDGETS_CYCLE_BOX_HPP
#define CPPURSES_WIDGET_WIDGETS_CYCLE_BOX_HPP
#include <string>
#include <vector>

#include <signals/signals.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/widgets/label.hpp>

namespace cppurses {

/// A rotating set of labels, emitting a Signal when the label is changed.
/** Labels are cycled forwards by a left click or scroll up, cycled backwards
 *  by a scroll down button press. */
class Cycle_box : public Label {
   public:
    Cycle_box();

    /// Add an option/label to the Cycle_box.
    /** The returned Signal reference will be emitted every time this option is
     *  enabled. */
    sig::Signal<void()>& add_option(Glyph_string option);

    /// Remove an option/label identified by its text.
    /** No-op if does no match found. Only removes the first option found if
     *  there are multiple options with identical text. */
    void remove_option(const Glyph_string& option);

    /// Return the current option's text.
    Glyph_string current_option() const;

    /// Move forward one option.
    void cycle_forward();

    /// Move backward one option.
    void cycle_backward();

    /// Emitted when the option is changed, sends the new option's string rep.
    sig::Signal<void(std::string)> option_changed;

   protected:
    bool mouse_press_event(const Mouse::State& mouse) override;

   private:
    /// Holds an Option's Signal and name.
    struct Option {
        explicit Option(Glyph_string name_);
        Glyph_string name;
        sig::Signal<void()> enabled;
    };
    std::vector<Option> options_;
};

namespace slot {

sig::Slot<void(Glyph_string)> add_option(Cycle_box& cb);
sig::Slot<void()> add_option(Cycle_box& cb, const Glyph_string& option);

sig::Slot<void(const std::string&)> remove_option(Cycle_box& cb);
sig::Slot<void()> remove_option(Cycle_box& cb, const std::string& option);

sig::Slot<void()> cycle_forward(Cycle_box& cb);
sig::Slot<void()> cycle_backward(Cycle_box& cb);

}  // namespace slot
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_CYCLE_BOX_HPP

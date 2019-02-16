#ifndef CPPURSES_WIDGET_WIDGETS_LINE_EDIT_HPP
#define CPPURSES_WIDGET_WIDGETS_LINE_EDIT_HPP
#include <functional>
#include <string>

#include <signals/signals.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/events/keyboard.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/widgets/textbox.hpp>

namespace cppurses {
struct Point;

// TODO input_mask option, which shows outline of acceptable text in ghost color
// ex) phone #| (___)___-____
// ip address| ___.___.___.___
// ref: webtoolkit.eu/widgets/forms/line_text-editor

/// Text input box with input validator and Signal emitted on Enter Key press.
/** Initial text is in ghost color and cleared on initial focus. Height is fixed
 *  at 1. */
class Line_edit : public Textbox {
   public:
    /// Construct a Line_edit object with \p initial_text in ghost color.
    explicit Line_edit(Glyph_string initial_text = "");

    /// Set the input validator, allowing or disallowing specific char types.
    /** The default validator always returns true, allowing all chars. Invalid
     *  character input will result in no input being given to the Line_edit. */
    void set_validator(std::function<bool(char)> validator);

    /// Set whether the text is cleared from the Line_edit on Enter Key press.
    /** This is disabled by default. */
    void clear_on_enter(bool enable = true);

    /// Set whether the Line_edit has invisible text, does not alter the output.
    /** Disabled by default. */
    void invisible_input(bool enabled = true);

    /// Set whether the Line_edit has an underline.
    /** Disabled by default. The entire length of the box is underlined if
     *  enabled. */
    void underline(bool enabled = true);

    /// Set color of the initial text, before focus has been given to this.
    void set_ghost_color(Color c);

    /// Emitted on Enter Key press, sends along the current contents.
    sig::Signal<void(std::string)> edit_finished;

   protected:
    bool key_press_event(const Keyboard::State& keyboard) override;
    bool mouse_press_event(const Mouse::State& mouse) override;
    bool focus_in_event() override;

   private:
    bool clear_on_enter_{false};
    bool on_initial_{true};
    std::function<bool(char)> validator_{[](char) { return true; }};
};
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_LINE_EDIT_HPP

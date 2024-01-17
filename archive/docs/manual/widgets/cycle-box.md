# Cycle Box Widget

[`<termox/widget/widgets/cycle_box.hpp>`](../../../include/termox/widget/widgets/cycle_box.hpp)

A rotating set of labels, emitting a Signal when the label is changed.  Labels
are cycled forwards by a left click or scroll up, cycled backwards by a scroll
down button press, or arrow keys.

```cpp
class Cycle_box : public HLabel {
   public:
    struct Parameters {
        Align alignment        = Align::Center;
        int extra_left         = 0;
        int extra_right        = 0;
        Growth growth_strategy = Growth::Static;
    };

    // Emitted when the option is changed, sends the new option's string rep.
    sl::Signal<void(std::string)> option_changed;

   public:
    // Given Label parameters are applied to each option's display.
    Cycle_box(Align alignment        = Align::Center,
              int extra_left         = 0,
              int extra_right        = 0,
              Growth growth_strategy = Growth::Static);

    Cycle_box(Parameters);

   public:
    // Add an option/label to the Cycle_box using Label constructor parameters.
    /* The returned Signal reference will be emitted every time this option is
     * enabled. \p label's string representation is the identifying param. */
    auto add_option(Glyph_string label) -> sl::Signal<void()>&;

    // Remove an option/label identified by its text.
    /* No-op if does no match found. Only removes the first option found if
     * there are multiple options with identical text. */
    void remove_option(Glyph_string const& label);

    // Move forward one option.
    void next();

    // Move backward one option.
    void previous();

    // Set the current option to the option with \p label identifier.
    /* Does not emit the option's signal. No-op if label is not an option. */
    void set_current_option(std::string const& label);

    // Set the current option to the one at \p index.
    /* Does not emit the option's signal.
     * Input Constraints: \p index < options_.size() */
    void set_current_option(std::size_t index);

    // Return the number of options in the Cycle_box.
    auto size() const -> std::size_t;
};
```

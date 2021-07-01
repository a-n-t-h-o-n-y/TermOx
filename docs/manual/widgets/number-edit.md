# Number Edit Widget

[`<termox/widget/widgets/number_edit.hpp>`](../../../include/termox/widget/widgets/number_edit.hpp)

A `Line_edit` type Widget, specifically for editing numbers, with scroll wheel
increment and keyboard up/down arrow input, a valid range and input validation.

```cpp
template <typename Number_t>
class Number_edit : public detail::Textline_base {
   public:
    struct Range {
        Number_t min = Min_num;
        Number_t max = Max_num;
    };

    struct Parameters {
        Number_t initial       = 0;
        Range range            = {};
        Align alignment        = Align::Left;
        Action focus_in_action = Action::None;
        Number_t increment     = 1;
    };

    // Emitted on Enter key press; sends along the current value.
    sl::Signal<void(Number_t)> submitted;

    // Emitted on key press and inc/decrement; sends along the current value.
    sl::Signal<void(Number_t)> value_modified;

   public:
    Number_edit(Number_t initial       = 0,
                Range range            = {},
                Align alignment        = Align::Left,
                Action focus_in_action = Action::None,
                Number_t increment     = 1);

    Number_edit(Parameters);

    // Set a new value to be displayed.
    /* Does not emit Signals. Clamps to range if outside of it. */
    void set_value(Number_t x);

    // Get the current value of the Number_edit, defaults to zero if invalid.
    auto value() const -> Number_t;

    // Set the accepted range of values
    void set_range(Range x);

    // Get the currently set Range.
    auto range() const -> Range;

    // Set the value to increment/decrement by on arrow key/mouse wheel input.
    void set_increment(Number_t x);

    // Return the current increment/decrement value.
    auto get_increment() const -> Number_t;

    // Increment the current value by the set increment.
    void increment();

    // Decrement the current value by the set increment.
    void decrement();
};

using Int_edit = Number_edit<int>;
using Unsigned_edit = Number_edit<unsigned int>;
using Double_edit = Number_edit<double>;
using Float_edit = Number_edit<float>;

```

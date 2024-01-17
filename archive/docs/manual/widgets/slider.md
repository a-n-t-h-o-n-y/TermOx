# Slider Widgets

[`<termox/widget/widgets/slider.hpp>`](../../../include/termox/widget/widgets/slider.hpp)

Fixed range sliding display fixed at one end of the Widget. Range can contain
negative values.

```cpp
template <template <typename> typename Layout_t, bool Inverted>
class Slider : public Widget {
   public:
    // [low, high] inclusive range of values.
    struct Range { int low; int high; };

    struct Parameters { Range range; int initial; };

    // Emitted when the [low, high] range is modified.
    sl::Signal<void(Range)> range_changed;

    // Emitted on user input and passes along the new current value set.
    sl::Signal<void(int)> value_changed;

   public:
    // Create new Slider.
    /* Throws std::invalid_argument if \p range's low is greater than its high.
     * Throws std::out_of_range if \p initial is outside of \p range. */
    Slider(Range range, int initial);

    // Create new Slider.
    /* Throws std::invalid_argument if \p range's low is greater than its high.
     * Throws std::out_of_range if \p initial is outside of \p range. */
    Slider(Parameters);

    // Set a new Range for this Slider.
    /* Throws std::invalid_argument if \p range's low is greater than its high.
     * The current value is clamped to the new range, if now outside of it. */
    void set_range(Range x);

    // Return the currently set Range. Emits range_changed Signal.
    auto range() const -> Range;

    // Set the current value of the Slider. Emits value_changed Signal.
    /* Throws std::out_of_range if outside of the currently set Range. */
    void set_value(int x);

    // Return the currently set value.
    auto value() const -> int;
};

using Slider_top    = Slider<layout::Vertical, false>;
using Slider_bottom = Slider<layout::Vertical, true>;
using Slider_left   = Slider<layout::Horizontal, false>;
using Slider_right  = Slider<layout::Horizontal, true>;
```

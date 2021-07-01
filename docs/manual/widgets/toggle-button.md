# Toggle Button Widget

[`<termox/widget/widgets/toggle_button.hpp>`](../../../include/termox/widget/widgets/toggle_button.hpp)

A Button with two alternating sides. The top button is active first, switching
between the two sides on left mouse presses.

```cpp
class Toggle_button : public SPair<Button, Button> {
   public:
    struct Parameters {
        Glyph_string top_text;
        Glyph_string bottom_text;
    };

    Button& top    = this->first;
    Button& bottom = this->second;

    sl::Signal<void()>& top_pressed    = top.pressed;
    sl::Signal<void()>& bottom_pressed = bottom.pressed;

   public:
    // Construct with corresponding labels.
    Toggle_button(Glyph_string top_text, Glyph_string bottom_text);

    Toggle_button(Parameters);

    // Display the top button, without emitting any Signals.
    void show_top();

    // Display the bottom button, without emitting any Signals.
    void show_bottom();

    // Change the displayed button without emitting any signals.
    void toggle();
};
```

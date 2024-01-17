# Textbox Widget

[`<termox/widget/widgets/textbox.hpp>`](../../../include/termox/widget/widgets/textbox.hpp)

Interactive `Glyph_string` display with text wrapping, alignment, etc...
Provides common text interaction and presentation options as well as proper
cursor movement for these interactions.

```cpp
class Textbox : public detail::Textbox_base {
   public:
    struct Parameters {
        Glyph_string text  = U"";
        Align alignment    = Align::Left;
        Wrap wrap          = Wrap::Word;
        Brush insert_brush = Brush{};
        int scroll_speed   = 1;
        bool text_input    = true;
    };

   public:
    // Construct a Textbox with initial \p contents and strong focus.
    Textbox(Glyph_string text  = U"",
            Align alignment    = Align::Left,
            Wrap wrap          = Wrap::Word,
            Brush insert_brush = Brush{},
            int scroll_speed   = 1,
            bool text_input    = true);

    Textbox(Parameters);

   public:
    // Set the number of lines scrolled vertically on scroll wheel events.
    /* Default value is one. Negative will invert. */
    void set_scroll_speed(int x);

    // Return the current scroll wheel speed.
    auto scroll_speed() const -> int;

    // Enable the Textbox to take keyboard input.
    void enable_text_input();

    // Disable the Textbox from taking keyboard input.
    void disable_text_input();

    // Return true if currently takes text keyboard input.
    auto has_text_input() const -> bool;
};
```

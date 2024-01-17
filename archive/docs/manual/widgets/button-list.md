# Button List Widget

[`<termox/widget/widgets/button_list.hpp>`](../../../include/termox/widget/widgets/button_list.hpp)

A list of Buttons with a connected scrollbar to move through the Buttons.

```cpp
template <template <typename> class Layout_t>
class Button_list
    : public Pair<layout::Opposite_t<Layout_t<Widget>>,
                  Scrollbar<Layout_t>,
                  detail::Just_a_button_list_and_buffer<Layout_t>> {
   public:
    // Emitted when a given button in the list is pressed.
    sl::Signal<void(std::u32string const& name)>& button_pressed =
        buttons.button_pressed;

    struct Parameters {
        Color scrollbar_bg = Color::Background;
        Color scrollbar_fg = Color::Foreground;
    };

   public:
    Button_list(Color scrollbar_bg = Color::Background,
                Color scrollbar_fg = Color::Foreground);

    Button_list(Parameters);

    // Returns reference to the Button added.
    /* The Brush and the 'pressed' signal are accessible to customize. */
    auto add_button(std::u32string const& name) -> Button&;

    // Set the background color of the Scrollbar.
    void set_scrollbar_bg(Color c);

    // Set the foreground color of the Scrollbar.
    void set_scrollbar_fg(Color c);
};

using VButton_list = Button_list<layout::Vertical>;
using HButton_list = Button_list<layout::Horizontal>;
```

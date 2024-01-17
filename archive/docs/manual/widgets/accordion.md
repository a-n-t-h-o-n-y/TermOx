# Accordion Widget

[`<termox/widget/widgets/accordion.hpp>`](../../../include/termox/widget/widgets/accordion.hpp)

Wrapper Widget that can collapse to a single line and expand back to its full
size.

```cpp
enum class Bar_position { First, Last };

template <template <typename> typename Layout_t,
          typename Widget_t,
          Bar_position position = Bar_position::First>
class Accordion : public Passive<Layout_t<Widget>> {
   public:
    // Parameters for the title display.
    struct Parameters {
        Glyph_string title;
        Align alignment = is_horizontal ? Align::Left : Align::Top;
        Glyph wallpaper = is_horizontal ? U'─' : U'│';
    };

   public:
    // Create an Accordion with \p args... going to Widget_t constructor.
    template <typename... Args>
    Accordion(Parameters p, Args&&... args);

   public:
    // Enable the wrapped Widget.
    void expand();

    // Disable the wrapped Widget.
    void collapse();

    void toggle_expansion();

    // Return the wrapped widget.
    auto wrapped() -> Widget_t&;
    auto wrapped() const -> Widget_t const&;

    // Return the titled Bar widget.
    auto bar() -> Bar_t&;
    auto bar() const -> Bar_t const&;
};

template <typename Widget_t, Bar_position position = Bar_position::First>
using HAccordion = Accordion<layout::Horizontal, Widget_t, position>;

template <typename Widget_t, Bar_position position = Bar_position::First>
using VAccordion = Accordion<layout::Vertical, Widget_t, position>;
```

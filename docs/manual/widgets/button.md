# Button Widgets

[`termox/widget/widgets/button.hpp`](../../../include/termox/widget/widgets/button.hpp)

## `Button`

Emits `pressed` signal on left mouse button press, and `released` on left mouse
button release. Constructor takes a `Glyph_string` which is centered
horizontally and vertically on the Widget.

```cpp
class Button : public Widget {
   public:
    sl::Signal<void()> pressed;
    sl::Signal<void()> released;

    struct Parameters { Glyph_string text = U""; };

   public:
    explicit Button(Glyph_string text = U"");
    explicit Button(Parameters);

    // Set the text and repaint.
    void set_text(Glyph_string text);

    // Return the current text.
    auto text() const -> Glyph_string const&;
};

auto button(Glyph_string text = U"") -> std::unique_ptr<Button>;
auto button(Button::Parameters) -> std::unique_ptr<Button>;
```

## `Push_button`

A Button that changes its background color on `pressed` signal, and resets the
color on `released` signal.

```cpp
class Push_button : public Button {
   public:
    struct Parameters {
        Glyph_string text;
        Color pressed_color  = Color::Foreground;
        Color released_color = Color::Background;
    };

   public:
    explicit Push_button(Glyph_string text    = U"",
                         Color pressed_color  = Color::Foreground,
                         Color released_color = Color::Background);

    explicit Push_button(Parameters);

   public:
    // Set the background Color of the Button when pressed Signal is emitted.
    void set_pressed_color(Color c);

    // Return the current Color assigned to mouse press events.
    auto get_pressed_color() const -> Color;

    // Set the background Color of the Button when released Signal is emitted.
    void set_released_color(Color c);

    // Return the current Color assigned to mouse release events.
    auto get_released_color() const -> Color;
};

auto push_button(Glyph_string text    = U"",
                 Color pressed_color  = Color::Foreground,
                 Color released_color = Color::Background)
    -> std::unique_ptr<Push_button>;

auto push_button(Push_button::Parameters)
    -> std::unique_ptr<Push_button>;
```

## `Thin_button`

A Button with a height or width of one. Type aliases `VThin_button` for a fixed
width of 1, and `HThin_button` for a fixed height of 1.

```cpp
template <template <typename> typename Layout_t>
class Thin_button : public Button {
   public:
    using Button::Parameters;

   public:
    explicit Thin_button(Glyph_string text = U"");
    explicit Thin_button(Parameters);
};

using HThin_button = Thin_button<layout::Horizontal>;
using VThin_button = Thin_button<layout::Vertical>;

template <template <typename> typename Layout_t>
auto thin_button(Glyph_string text = U"") -> std::unique_ptr<Thin_button<Layout_t>>;

template <template <typename> typename Layout_t>
auto thin_button(typename Thin_button<Layout_t>::Parameters) -> std::unique_ptr<Thin_button<Layout_t>>;

auto hthin_button(Glyph_string text = U"") -> std::unique_ptr<HThin_button>;

auto hthin_button(HThin_button::Parameters) -> std::unique_ptr<HThin_button>;

auto vthin_button(Glyph_string text = U"") -> std::unique_ptr<VThin_button>;

auto vthin_button(VThin_button::Parameters) -> std::unique_ptr<VThin_button>;
```

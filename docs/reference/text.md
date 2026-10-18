# Labels and text editing

[API reference](../reference.md) · [Guides](../README.md)

Labels, numeric labels, text widgets, and the editing core.

## On this page

- [ox::Label](#oxlabel)
- [ox::BasicNumberLabel ox::IntegerLabel ox::FloatLabel](#oxbasicnumberlabel-oxintegerlabel-oxfloatlabel)
- [ox::LineEdit](#oxlineedit)
- [ox::TextBox](#oxtextbox)
- [Text editing](#text-editing)

## ox::Label

[`#include <ox/label.hpp>`](../../include/ox/label.hpp)

`class Label : public Widget;`

A Widget displaying one horizontal line or a vertical stack of complete Unicode
graphemes. `.vertical` defaults to false. Alignment is along the text direction and
the cross axis is centered. Use `set_text()` and `set_vertical()` to request layout
and repaint, or call `refresh()` after public configuration edits. See
[label orientation](../labels.md#orientation-and-alignment) for sizing and clipping.

```cpp
auto label = Label{{
    .text = "A Label",
    .align = Align::Left,
    .style = {.foreground = colors::bright_white},
}};
```

### Constructors

```cpp
struct Options {
    std::string text = "";
    bool vertical = false;
    Align align = Align::Center;
    Style style = {};
    SizePolicy width_policy = vertical ? SizePolicy{.minimum = 1, .flexibility = 0}
                                       : SizePolicy::flex();
    SizePolicy height_policy = vertical ? SizePolicy::flex() : SizePolicy::fixed(1);
};

Label(Options x = {});

Label(std::string text_);

Label(char const* text_);
```

The `std::string` and `char const*` constructors are provided for convenience, they will
set the `text` member to the provided value and all other options to their default
values.

The `Options` constructor takes a `std::string` for the text, an `Align` enum for the
alignment along the text direction, a `Style` to apply to each cell of the Label, and a `SizePolicy`.

### Public members

```cpp
std::string text;
bool vertical;
Align align;
Style style;
```

The text to be displayed, the alignment of the text, and the Style to use when painting.

## ox::BasicNumberLabel ox::IntegerLabel ox::FloatLabel

[`#include <ox/label.hpp>`](../../include/ox/label.hpp)

```cpp
template <typename NumberType>
class BasicNumberLabel : public Widget;

using IntegerLabel = BasicNumberLabel<int>;
using FloatLabel = BasicNumberLabel<float>;
```

A Widget displaying a number of a given type. The number is not editable, but it can be
set directly via its public data members. The number is centered vertically, its
horizontal alignment is determined by the `Align` constructor parameter, and its
formatting is determined by a format string and locale.

```cpp
auto label = IntegerLabel{{
    .value = 42,
    .align = Align::Left,
    .style = {.foreground = colors::bright_white},
    .locale = digit_separator_locale(3),
    .format = "{:d}",
}};
```

### Constructors

```cpp
struct Options {
    NumberType value = 0;
    Align align = Align::Center;
    Style style = {};
    std::locale locale = std::locale{};
    std::string format = "{}";
    SizePolicy width_policy = SizePolicy::flex();
    SizePolicy height_policy = SizePolicy::fixed(1);
};

BasicNumberLabel(Options x = {});
```

Construct a BasicNumberLabel Widget. The `value` is the number to be displayed, `align`
is the horizontal alignment of the text, the `style` is the Style to use when painting,
the `locale` is the locale to use for formatting, and the `format` is the format string
to use for formatting.

### Public members

```cpp
NumberType value;
Align align;
Style style;
std::locale locale;
std::string format;
```

The number to be displayed, the alignment of the text, the Style to use when painting,
the locale to use for formatting, and the format string to use for formatting.

### `digit_separator_locale` Free Function

```cpp
auto digit_separator_locale(std::int8_t spacing = 3,
                            std::locale init_locale = std::locale{})
    -> std::locale;
```

Creates a locale that inserts a digit separator character. The `spacing` parameter
determines the number of digits between separators. The `init_locale` parameter is the
locale to base the new locale on. The default is the C locale. The function will throw
a `std::invalid_argument` exception if the `spacing` parameter is negative.

## ox::LineEdit

[`#include <ox/lineedit.hpp>`](../../include/ox/lineedit.hpp)

`class LineEdit : public Widget;`

A single-line input with Unicode selection, scrolling, undo, clipboard commands,
read-only behavior, placeholders, and complete-value validation.

```cpp
auto input = LineEdit{{
    .text = "123",
    .style = {.foreground = colors::bright_white},
    .placeholder = "Number",
    .accept_character = [](char32_t c) { return c >= U'0' && c <= U'9'; },
}};
input.on_submit.connect([](std::string const& text) { /* Use the value. */ });
```

See [text editing](../text-editing.md#editing-commands) for the shared
LineEdit/TextBox mutation commands, signals, validation, and viewport API.

## ox::TextBox

[`#include <ox/textbox.hpp>`](../../include/ox/textbox.hpp)

`class TextBox : public Widget;`

Multiline input with the same editing commands/signals as LineEdit. Word wrapping is
the default; `TextWrap::None` enables horizontal scrolling. Ctrl-Enter submits. Set
`read_only` for selectable, noneditable text independently of focus policy.

```cpp
auto text = TextBox{{.text = "A TextBox", .wrap = TextWrap::Word}};
auto bar = ScrollBar{};
auto connections = link(text, bar);
```

`link` returns the two signal identifiers; it tracks widget moves and lifetimes.
See [text editing](../text-editing.md#editing-commands).

## Text editing

[`#include <ox/core/text.hpp>`](../../include/ox/core/text.hpp)

`TextEdit` owns UTF-8 text, grapheme-boundary carets, selection, bounded undo/redo,
and atomic paste assembly. `TextLayout` owns cached display rows and width assumptions;
`layout_text()` is the pure calculation. Byte offsets and terminal columns remain
explicitly separate. LineEdit and TextBox compose these with the functions in `<ox/text_view.hpp>`.

See [text editing internals](../internals/text.md) for normalization, navigation, history, rendering,
and the new widget text accessors.

# API Reference

See [theme and motion](theme-and-motion.md) for `Theme`, `MotionPolicy`, `Tween`,
`Animation`, fractional meters, box-drawing rules, and `KeyHelp`.

## 🧩 ox::Application

[`#include <ox/application.hpp>`](../include/ox/application.hpp)

An application owns its event loop, focus, and widget interaction. Construction does
not open a terminal. The root widget must outlive the application.

```cpp
auto app = Application{head};
return app.run({.mouse_mode = MouseMode::Move});
```

`step(now)` dispatches posted events and due timers without terminal I/O. `paint()`
paints a pending frame; `loop().screen` contains its cells. `post(event)` accepts events
from background threads. `request_repaint()` and `quit(code)` operate on the application
thread. `dispatch(event)` delivers immediately and applies effects; raw `handle`
methods return effects. `set_focus(widget)` and `clear_focus()` are commands;
`focus()` provides read-only access to the manager.

See [the event loop guide](event-loop.md) for the response contract and scheduling.

## 🧩 ox::Timer

[`#include <ox/timer.hpp>`](../include/ox/timer.hpp)

A movable timer handle using the event loop's steady-clock scheduler. Timers create no
threads. Retain the handle to retain its schedule.

```cpp
Timer(std::chrono::milliseconds interval,
      std::function<EventResponse()> callback,
      bool launch = true);
// For any derived widget W and callback returning EventResponse:
Timer(W& widget, std::chrono::milliseconds interval, Callback callback,
      bool launch = false);
```

Widget callbacks receive `W&` at execution time, follow moves, and are adopted by the
application. Register generic callback timers with `loop.watch(timer)`. Intervals must
be positive and callbacks nonempty. `start(now)` resets the deadline; `stop()` cancels
it. `is_running()` reports whether a deadline exists. Delays deliver one tick and
skip missed ticks. Multiple widget timers have independent callbacks.

## 🧩 ox::Focus

[`#include <ox/focus.hpp>`](../include/ox/focus.hpp)

`Focus(root)` manages one tree and returns transition effects without applying them.
`app.set_focus(widget)` and `app.clear_focus()` apply those effects at the application
boundary. `app.focus().get()` returns a lifetime view; `Widget::is_focused()` reads
focus state. See [routing and focus](routing-and-focus.md).

## 📦 ox::shape::HLine

[`#include <ox/put.hpp>`](../include/ox/put.hpp)

`struct HLine;`

A horizontal line that you can paint to a Canvas with `put(...)`.

```cpp
auto length = 5;
put(c, {.x = 5, .y = 2}, shape::HLine{.symbol = U'-'}, length);
```

<details>
<summary><strong>Details</strong></summary>

```cpp
struct HLine {
    char32_t symbol = U'─';
};
```

---

</details>

## 📦 ox::shape::VLine

[`#include <ox/put.hpp>`](../include/ox/put.hpp)

`struct VLine;`

A vertical line that you can paint to a Canvas with `put(...)`.

```cpp
auto length = 5;
put(c, {.x = 5, .y = 2}, shape::VLine{.symbol = U'|'}, length);
```

<details>
<summary><strong>Details</strong></summary>

```cpp
struct VLine {
    char32_t symbol = U'│';
};
```

---

</details>

## 📦 ox::shape::Box

[`#include <ox/put.hpp>`](../include/ox/put.hpp)

`struct Box;`

A 2D rectangle outline that you can paint to a Canvas with `put(...)`.

```cpp
auto size = Area{.width = 10, .height = 5};
put(c, {.x = 5, .y = 2}, shape::Box::square(), size);
```

<details>
<summary><strong>Details</strong></summary>

```cpp
struct Box {
    /// {top left, top right, bottom left, bottom right}
    std::array<char32_t, 4> corners;

    /// {N, S, E, W}
    std::array<char32_t, 4> walls;

    static constexpr Box square();
    static constexpr Box half_square();
    static constexpr Box round();
    static constexpr Box half_round();
    static constexpr Box bold();
    static constexpr Box dashed();
    static constexpr Box dotted();
    static constexpr Box ascii();
    static constexpr Box half_twin();
    static constexpr Box twin();
    static constexpr Box half_bold();
    static constexpr Box corners_square();
    static constexpr Box corners_round();
    static constexpr Box brackets();
    static constexpr Box brackets_round();
    static constexpr Box brackets_bold();
    static constexpr Box brackets_twin();
    static constexpr Box twin_horizontal();
    static constexpr Box twin_vertical();
};
```

---

</details>

## 🔧 ox::put(...)

[`#include <ox/put.hpp>`](../include/ox/put.hpp)

A function overload set to put Glyphs on a Canvas object.

```cpp
auto paint(Canvas c) -> Cursor override {
    put(c, {.x = 5, .y = 10}, 'X');
    put(c, {.x = 6, .y = 10}, 'X' | fg(colors::blue));
    put(c, {.x = 7, .y = 10}, "Hello!");

    put(c, {.x = 0, .y = 0}, shape::HLine{}, this->size.width);
    put(c, {.x = 0, .y = 0}, shape::VLine{}, 3, Style{.background = RGB{0x337283}});
    put(c, {.x = 0, .y = 1}, shape::Box::dashed(), {.width = 10, .height = 4});

    put(c, shape::Box::bold());  // Outlines the Canvas.

    return {};
}
```

<details>
<summary><strong>Details</strong></summary>

A Canvas is a light-weight 2D grid of Glyph objects. It provides a direct mapping to a
sub-region of the `Frame`.

`put(...)` functions do bounds checking, and anything painted out of bounds of the
Canvas will be ignored. These functions are provided as convenience, as an alternative
with no bounds checking, individual Glyphs can be painted to the screen with
`Canvas::operator[]`. A Widget's Canvas starts at the top left with `Point{.x = 0, .y =
0}` and ends at the bottom right with `Point{.x = this->size.width - 1, .y =
this->size.height - 1}`.

Overloads without a `Color` or `Style` will modify the `Glyph::symbol` members in the
Canvas, keeping the existing Style. Overloads with `Color` will overwrite both the
symbol and the `Glyph::style::foreground` members, and overloads with `Style` will
overwrite the symbol and the `Glyph::style` members.

```cpp
void put(Canvas c, Point at, Glyph const& item);
void put(Canvas c, Point at, Character auto item);
void put(Canvas c, Point at, GlyphRange auto const& item);
void put(Canvas c, Point at, std::string_view element);
void put(Canvas c, Point at, std::u32string_view element);

void put(Canvas c, Point at, shape::HLine item, int length);
void put(Canvas c, Point at, shape::HLine item, int length, Color foreground);
void put(Canvas c, Point at, shape::HLine item, int length, Style const& style);

void put(Canvas c, Point at, shape::VLine item, int length);
void put(Canvas c, Point at, shape::VLine item, int length, Color foreground);
void put(Canvas c, Point at, shape::VLine item, int length, Style const& style);

void put(Canvas c, Point at, shape::Box const& item, Area size);
void put(Canvas c, Point at, shape::Box const& item, Area size, Color foreground);
void put(Canvas c, Point at, shape::Box const& item, Area size, Style const& style);

void put(Canvas c, shape::Box const& item);
void put(Canvas c, shape::Box const& item, Color foreground);
void put(Canvas c, shape::Box const& item, Style const& style);
```

---

</details>

## 🔧 ox::fill(...)

[`#include <ox/put.hpp>`](../include/ox/put.hpp)

A function overload set to fill a 2D space on a Canvas. It fills the entire Canvas
object given to it.

```cpp
auto paint(Canvas c) -> Cursor override {
    fill(c, 'X');
    fill(c, fg(RGB{0x33A421}));
    fill(c, Trait::Bold | Trait::Standout);

    auto sub_region = Canvas{
        .buffer = c.buffer,
        .at = c.at + {.x = 3, .y = 0}, 
        .size = {.width = 5, .height = 7},
    };
    fill(sub_region, 'X' | bg(colors::red) | fg(colors::black));

    return {};
}
```

<details>
<summary><strong>Details</strong></summary>

The overloads are similar to `put(...)`, where the Glyph overload will overwrite
everything, the `char32_t` overload will only overwrite the `Glyph::symbol` member,
etc... To fill a sub-section of the Canvas, create a new Canvas object with the desired
dimensions and position. 

```cpp
void fill(Canvas c, Glyph g);
void fill(Canvas c, Style b);
void fill(Canvas c, char32_t ch);
void fill(Canvas c, ColorBG bg);
void fill(Canvas c, ColorFG fg);
void fill(Canvas c, Traits ts);
```

---

</details>

## 🔧 ox::clear(...)

[`#include <ox/put.hpp>`](../include/ox/put.hpp)

Resets all Glyphs in a Canvas to the default empty Glyph.

```cpp
auto paint(Canvas c) -> Cursor override {
    fill(c, fg(RGB{0x33A421}));
    clear(c);

    return {};
}
```

<details>
<summary><strong>Details</strong></summary>

This function should not be needed in most circumstances, the Canvas is provided cleared
on entry to the `Widget::paint(Canvas)` event handler, unless a subclass or parent
Widget has written over it.

```cpp
void clear(Canvas c);
```

---

</details>

## 🧩 ox::Widget

[`#include <ox/widget.hpp>`](../include/ox/widget.hpp)

A move-only base for input and drawing. Configuration is public; geometry, focus,
and lifetime ownership are controlled by the core. See [the widget API guide](widget-api.md)
for a complete example and migration table.

```cpp
FocusPolicy focus_policy;
SizePolicy width_policy;
SizePolicy height_policy;
bool visible = true;
bool enabled = true;

Rect bounds() const;
Point at() const;
Area size() const;
bool is_focused() const;

virtual auto handle(KeyEvent const&) -> EventResponse;
virtual auto handle(MousePress const&) -> EventResponse;
virtual auto handle(MouseRelease const&) -> EventResponse;
virtual auto handle(MouseMove const&) -> EventResponse;
virtual auto handle(MouseWheel const&) -> EventResponse;
virtual auto handle(esc::PasteChunk const&) -> EventResponse;
virtual auto paint(Canvas) -> Cursor;

// Hover and focus notifications return EventResponse:
virtual auto mouse_enter() -> EventResponse;
virtual auto mouse_leave() -> EventResponse;
virtual auto focus_in() -> EventResponse;
virtual auto focus_out() -> EventResponse;

virtual auto get_children() -> zzz::Generator<Widget&>;
virtual auto get_children() const -> zzz::Generator<Widget const&>;

// Protected layout hook; bounds have already changed:
virtual void on_arrange(Rect previous);
```

Default input and lifecycle handlers return an empty response; default paint returns
no cursor. Press, repeat, and release actions use the same key handler. Use
`using Widget::handle;` when a derived class needs direct access to inherited overloads.

`arrange(widget, bounds)` validates dimensions, updates parent-local geometry,
requests painting if attached, and calls the hook even when bounds are unchanged.
Parents arrange children with this function. Paint returns a local cursor; only the
focused widget's returned cursor is presented. No persistent cursor field is stored.

Public configuration and widget-specific state can be edited directly. Arrange the
parent again after changing layout policies. Request painting after changes that are
not detected by tree synchronization. Geometry and focus cannot be written directly.
Move operations transfer lifetime tracking; assignment invalidates the overwritten
identity. `track(widget)` provides safe lookup without exposing lifetime storage.

## 📦 ox::Connection

[`#include <ox/widget.hpp>`](../include/ox/widget.hpp)

```cpp
auto id = ox::Connection{
    .signal = numberpad.on_press,
    .slot = [](int n, auto& total_label) { total_label.value += n; },
}(total_label);
// Optional explicit disconnection:
numberpad.on_press.disconnect(id);
```

`Connection{.signal = signal, .slot = callback}(widgets...)` connects a signal to a callback.
Signal arguments come first, followed by references to tracked widgets. Tracking borrows
identity, follows moves, permits const targets, and suppresses invocation after
destruction, overwrite, or slicing. Empty callbacks are rejected before subscription.
Calls and widget mutations belong to the application thread.

The signal owns its callback. Discarding the returned `sl::Identifier` leaves the
connection active; it is not an RAII handle. The callback's captures remain owned until
the signal disconnects the slot or is destroyed, even when a tracked target expires.
Explicit disconnection returns the slot; retaining that slot retains its captures.
Constructing the configuration does not connect it; invoke it immediately to subscribe.

## 🔢 ox::FocusPolicy

[`#include <ox/widget.hpp>`](../include/ox/widget.hpp)

Provides a description of how a Widget can attain Focus. Focus allows a Widget to
recieve key press events. Used by the Application class on mouse clicks and tab presses
to determine the next focus Widget.

```cpp
TextBox{} | FocusPolicy::Strong; // Enable editing in a TextBox by providing Focus.
```

<details>
<summary><strong>Details</strong></summary>

The pipe (`|`) operator is overloaded for Widget and FocusPolicy to set the FocusPolicy
inline with Widget construction.

```cpp
enum class FocusPolicy : std::uint8_t { None, Tab, Click, Strong };
```

- **None**: The Widget will never recieve Focus.
- **Tab**: A tab or shift + tab key press will give it Focus, if it is next in order for
Tab Focus.
- **Click**: A left mouse click on the Widget will give it Focus.
- **Strong**: Both left mouse click and tab key press will give it Focus.

---

</details>

## 📦 ox::SizePolicy

[`#include <ox/core/layout.hpp>`](../include/ox/core/layout.hpp)

Provides a description of how a Widget can be sized within its parent layout.

```cpp
Column{
    Label{"A TextBox"} | SizePolicy::fixed(1),
    TextBox{} | SizePolicy::flex(),
    Label{"Footer"} | SizePolicy::fixed(5),
};
```

<details>
<summary><strong>Details</strong></summary>

A Row uses each child's policy for width; a Column uses it for height. Suspended
takes a requested Area directly. See [layout calculation](layout.md) for proportional
shrinking, weighted growth, largest-remainder rounding, and input validation.

Pipe (`|`) operator overloading provided for Widgets to set their SizePolicy.

```cpp
struct SizePolicy {
    int minimum = 0;
    int maximum = std::numeric_limits<int>::max();
    float flexibility = 1.f;

    static constexpr auto fixed(int size) -> SizePolicy;
    static constexpr auto flex(float flex = 1.f) -> SizePolicy;
    static constexpr auto bounded(int min, int max) -> SizePolicy;
    static constexpr auto min(int min) -> SizePolicy;
    static constexpr auto max(int max) -> SizePolicy;
};
```

---

</details>

## 🧩 ox::LifetimeView

[`#include <ox/widget.hpp>`](../include/ox/widget.hpp)

`track(widget)` creates a weak view that follows moves and expires on destruction or
overwrite. Storage is private. `try_get()` returns a typed pointer or null without
throwing; `get()` returns a reference or throws `std::logic_error`; `valid()` tests
availability. Typed views reject a target whose derived type was lost through slicing.

```cpp
auto view = track(widget);
if (auto* target = view.try_get()) { target->enabled = false; }
```

`tracked(callback, widgets...)` invokes a callback only while every target is valid,
passing tracked widgets after the invocation's ordinary arguments. `Connection`
combines this with signal connection. Views and widget mutation belong to the
application thread.

## 🔢 ox::Align

[`#include <ox/align.hpp>`](../include/ox/align.hpp)

`Align` describes alignment along the text direction: `Start`, `Center`, or `End`.
For horizontal text these mean left/center/right; for vertical labels they mean
top/middle/bottom. `Left` and `Right` are aliases for Start and End.

```cpp
Label{{.text = "Hello", .align = Align::Center}};
auto offset = align_offset(Align::Center, 10, 4); // 3 terminal columns.
```

`align_offset` requires nonnegative available/used terminal-column counts. Oversized
content can produce a negative offset. Invalid enum values throw `std::invalid_argument`.
The unused VAlign enum has been removed; centered layout geometry uses `centered_rect`.

## 🧩 Mouse Events

TermOx uses Escape's `MousePress`, `MouseRelease`, `MouseMove`, and `MouseWheel`
types directly. Each event carries `at`, relative to the receiving widget, and
`modifiers`. Press events have a `MouseButton`; releases have an optional button,
and moves have an optional `held_button`. A missing release button means the
terminal did not identify the released button.

Wheel events use signed `dx` and `dy` steps: negative means left or up.

```cpp
auto mouse_press(MousePress m) -> EventResponse override
{
    if (m.button != MouseButton::Left) { return {}; }
    // Apply the click at m.at.
    return {.handled = true, .repaint = true};
}
```

## 🧩 ox::KeyEvent

TermOx uses Escape's `KeyEvent` directly for `handle(KeyEvent const&)`.
`Key` is a variant of `char32_t`, `KeyCode`, and `esc::KittyKey`. An event has
an optional key identity, `Modifiers`, a `KeyAction`, and associated UTF-32 text.
An IME event can carry text without a key identity.

```cpp
auto handle(KeyEvent const& event) -> EventResponse override
{
    if (event.action == KeyAction::Release) { return {}; }
    if (event.matches(KeyCode::Enter, {}, event.action)) {
        // Accept the current selection.
        return {.handled = true, .repaint = true};
    }
    if (event.matches(U'q', {}, event.action)) { return {.handled = true, .quit = 0}; }
    return {};
}
```

Press, repeat, and release actions reach the same overload; inspect `event.action`.
`input_text(event)` returns printable text for editing, excluding releases and
control shortcuts. `handle(esc::PasteChunk const&)` receives bracketed paste chunks.
Text widgets buffer paste bytes until the final chunk so a UTF-8 scalar split
between chunks is decoded correctly. Aborted pastes are discarded.

---

## 🧩 ox::Border

[`#include <ox/bordered.hpp>`](../include/ox/bordered.hpp)

`struct Border;`

Visual description of a Border surrounding a Widget. To be used with `Bordered` class.

```cpp
struct Border {
    shape::Box box = shape::Box::square();
    Style style = {};  // Only applies to box.
    Label::Options label = {};
};
```

<details>
<summary><strong>Details</strong></summary>

The `Border::style` member only applies to the drawing of the Box. The Label has its own
internal Style it will use to paint its text. Alignment of the Label will properly align
along the top of the border. If no text is set on the Label, it will display only the
Box.

---

</details>

## 🧩 ox::Bordered

[`#include <ox/bordered.hpp>`](../include/ox/bordered.hpp)

```cpp
template <WidgetDerived ChildWidget>
class Bordered : public Widget;
```

Wraps a Widget in a Border. The child receives `border_content_rect(available)`, inset
one cell from each edge. Tiny areas can leave an empty child interior.

This is technically a layout, the wrapped Widget is accessed via the `Bordered::child` member.

A Widget can be easily wrapped in a Border by using the `operator|` overload:

```cpp
template <WidgetDerived WidgetType>
auto operator|(WidgetType w, Border b) -> Bordered<WidgetType>;
```

The `operator|` overload will create a new Bordered object that will take ownership of
the passed in Widget.

```cpp
    auto bordered = MyWidget{} | Border{.box = shape::Box::round(), .label = {.text = "A Label"}};
    auto& my_widg = bordered.child;
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
struct Options {
    ChildWidget child = {};
    Border border = {};
};

Bordered(Options x);
```

Takes ownership of the child Widget.

---

### Public Objects

```cpp
ChildWidget child;
Border border;
```

Access to the wrapped child Widget and Border.

---

### `get_child` Free Function

```cpp
template <std::size_t I, WidgetDerived WidgetType>
auto get_child(Bordered<WidgetType>& bordered) -> auto&;
```

Returns the `I`th child of the wrapped Widget, provided for convenience and uniformity with other Layouts.

---

</details>

## 🧩 ox::Lines

[`#include <ox/bordered.hpp>`](../include/ox/bordered.hpp)

`struct Lines;`

Visual line data, used by Divider class.

```cpp
struct Lines {
    char32_t vertical;
    char32_t horizontal;

    static constexpr auto light() -> Lines { return {U'│', U'─'}; }
    static constexpr auto bold() -> Lines { return {U'┃', U'━'}; }
    static constexpr auto twin() -> Lines { return {U'║', U'═'}; }
    static constexpr auto dashed() -> Lines { return {U'╎', U'╌'}; }
    static constexpr auto dotted() -> Lines { return {U'┆', U'┄'}; }
    static constexpr auto ascii() -> Lines { return {U'|', U'-'}; }
    static constexpr auto empty() -> Lines { return {U' ', U' '}; }
};
```

## 🧩 ox::Divider

[`#include <ox/bordered.hpp>`](../include/ox/bordered.hpp)

`class Divider : public Widget;`

A line widget with explicit `vertical` direction (false by default) and an overlaid
plain label. Vertical labels stack graphemes along the line. See
[decorations and layouts](decorations-layouts.md) for geometry, clipping, and migration.

```cpp
Column{
    Button{},
    Divider{{
        .lines = Lines::bold(),
        .style = {.foreground = colors::bright_black},
    }},
    Button{},
};
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
struct Options {
    bool vertical = false;
    Lines lines = Lines::light();
    Style style = {};
    Label::Options label = {};
};

Divider(Options x);
```

The `Divider::style` member only applies to the Line, and not the Label, The Label has
its own internal Style.

---

### Public Objects

```cpp
bool vertical;
Lines lines;
Style style;
Label::Options label;
```

---

</details>

## 🧩 ox::Button

[`#include <ox/button.hpp>`](../include/ox/button.hpp)

`class Button : public Widget;`

Button composes a label, style patches, and optional animation. Connect actions to
`on_activate`; Enter/Space presses activate once, and a captured mouse gesture activates
on release inside. Repeats do not activate. `on_press`/`on_release` describe mouse gestures.
Read state through `is_pressed()` and `is_hovered()`.

```cpp
auto button = Button{{.label = {.text = "Run"}}};
button.on_activate.connect([] { /* Run the action. */ });
```

## 🧩 ox::CheckBox

`<ox/checkbox.hpp>` supplies a checkbox with a plain label caption.
Read `checked()`; mutate with `set_checked(bool)` or `toggle()`. `on_change(bool)` emits
only for changes. Glyph vectors and focused style remain public configuration.

## 🧩 ox::RadioGroup

`<ox/radiogroup.hpp>` supplies a vertical group with owned item strings. Read `items()`,
`selected_index()`, and `highlighted_index()`; mutate with `set_items` and `select`.
`select(std::nullopt)` clears selection; invalid indices throw. Arrows move and select,
while Enter/Space or click selects. `on_select(optional<size_t>)` emits only for changes.
See [control composition](controls.md) for contracts and migration.

## 🧩 ox::DataTable

A rectangular table with owned row vectors, column descriptions, fixed headers,
Unicode-safe ellipsis, two-axis scrolling, keyboard/mouse row selection, activation,
and stable sorting. Columns compose SizePolicy, Align, and StylePatch. Data access is
through const spans; mutation functions validate shape and UTF-8 before committing.

See [data tables and scrolling](data-tables.md) for the complete API, examples,
callbacks, sorting/filtering, viewport rules, and migration. The interactive example
is [examples/datatable.cpp](../examples/datatable.cpp).

## 🧩 ox::Label

[`#include <ox/label.hpp>`](../include/ox/label.hpp)

`class Label : public Widget;`

A Widget displaying one horizontal line or a vertical stack of complete Unicode
graphemes. `.vertical` defaults to false. Alignment is along the text direction and
the cross axis is centered. Use `set_text()` and `set_vertical()` to request layout
and repaint, or call `refresh()` after public configuration edits. See
[label orientation](decorations-layouts.md#label-orientation) for sizing and clipping.

```cpp
auto label = Label{{
    .text = "A Label",
    .align = Align::Left,
    .style = {.foreground = colors::bright_white},
}};
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

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

---

### Public Objects

```cpp
std::string text;
bool vertical;
Align align;
Style style;
```

The text to be displayed, the alignment of the text, and the Style to use when painting.

---

</details>

## 🧩 ox::BasicNumberLabel ox::IntegerLabel ox::FloatLabel

[`#include <ox/label.hpp>`](../include/ox/label.hpp)

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

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

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

---

### Public Objects

```cpp
NumberType value;
Align align;
Style style;
std::locale locale;
std::string format;
```

The number to be displayed, the alignment of the text, the Style to use when painting,
the locale to use for formatting, and the format string to use for formatting.

---

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

---

</details>

## Layout Concepts

[`#include <ox/layout.hpp>`](../include/ox/layout.hpp)

A few concepts are provided to help identify the types of containers that can be used
within Row and Column layouts.

```cpp
/**
 * tuple, pair, array, etc...
 */
template <typename T>
concept TupleLike = requires {
    { std::tuple_size<T>::value } -> std::convertible_to<std::size_t>;
};

/**
 * vector, set, list, etc...
 */
template <typename T>
concept DynamicContainer = not TupleLike<std::remove_reference_t<T>> &&
                           std::ranges::sized_range<T> && std::ranges::forward_range<T>;

template <typename T>
concept LayoutContainer = DynamicContainer<T> || TupleLike<T>;

template <typename T>
concept WidgetDerived = std::is_base_of_v<Widget, T>;
```

## 🧩 ox::Column

Call `refresh()` after changing children, visibility, or policies. See
[decorations and layouts](decorations-layouts.md#row-and-column-refresh) for caching
and mutation/callback behavior.

[`#include <ox/layout.hpp>`](../include/ox/layout.hpp)

```cpp
template <LayoutContainer Container>
class Column : public Widget;
```

A layout that arranges its children in a vertical column. The children are arranged from
top to bottom, with the first child of the container at the top and the last child at
the bottom. Each child is given the same width as the Column, and its height is
determined by the child's SizePolicy.

The Column layout is a `Widget` itself, so it can be used within other layouts as a
child.

```cpp
auto col = Column{
    Label{"A Label"} | SizePolicy::fixed(1),
    TextBox{} | SizePolicy::flex(),
    Label{"Footer"} | SizePolicy::fixed(5),
};

auto col2 = Column<std::vector<Button>>{};
col2.children.push_back(Button{{.label = {.text = "A Button"}}} | SizePolicy::fixed(5));
col2.children.push_back(Button{{.label = {.text = "B Button"}}} | SizePolicy::flex());
col2.children.push_back(Button{{.label = {.text = "C Button"}}} | SizePolicy::fixed(5));
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
explicit Column(Container container = {});

template <WidgetDerived... Widgets>
explicit Column(Widgets&&... ws);
```

The first constructor takes a container of Widgets, which can be a tuple, pair, array,
or any other container that meets the `LayoutContainer` concept.

The second constructor takes a variadic list of Widgets and moves them into a
`std::tuple` container. This is provided for convenience.

---

### Public Objects

```cpp
Container children;
```

Direct access to the children of the Column layout.

---

### `get_child` Free Function

```cpp
template <std::size_t I, TupleLike Container>
auto get_child(Column<Container>& row) -> auto&
```

Returns the `I`th child of the Column layout. This is provided for convenience and
uniformity with other Layouts. This is only available for `std::tuple` container
layouts.

---

</details>

## 🧩 ox::Row

Call `refresh()` after changing children, visibility, or policies. Const get_child
and ordinary tuple/array/vector/list composition are supported. See
[decorations and layouts](decorations-layouts.md#row-and-column-refresh).

[`#include <ox/layout.hpp>`](../include/ox/layout.hpp)

```cpp
template <LayoutContainer Container>
class Row : public Widget;
```
A layout that arranges its children in a horizontal row. The children are arranged from
left to right, with the first child of the container on the left and the last child on
the right. Each child is given the same height as the Row, and its width is determined
by the child's SizePolicy.

The Row layout is a `Widget` itself, so it can be used within other layouts as a child.

```cpp
auto row = Row{
    Button{{.label = {.text = "A Button"}}} | SizePolicy::fixed(5),
    TextBox{} | SizePolicy::flex(),
    Button{{.label = {.text = "B Button"}}} | SizePolicy::fixed(5),
};

auto row2 = Row<std::vector<Button>>{};
row2.children.push_back(Button{{.label = {.text = "A Button"}}} | SizePolicy::fixed(5));
row2.children.push_back(Button{{.label = {.text = "B Button"}}} | SizePolicy::flex());
row2.children.push_back(Button{{.label = {.text = "C Button"}}} | SizePolicy::fixed(5));
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
explicit Row(Container container = {});

template <WidgetDerived... Widgets>
explicit Row(Widgets&&... ws);
```

The first constructor takes a container of Widgets, which can be a tuple, pair, array,
or any other container that meets the `LayoutContainer` concept.

The second constructor takes a variadic list of Widgets and moves them into a
`std::tuple` container. This is provided for convenience.

---

### Public Objects

```cpp
Container children;
```

Direct access to the children of the Row layout.

---

### `get_child` Free Function

```cpp
template <std::size_t I, TupleLike Container>
auto get_child(Row<Container>& row) -> auto&
```

Returns the `I`th child of the Row layout. This is provided for convenience and
uniformity with other Layouts. This is only available for `std::tuple` container
layouts.

---

</details>

## 🧩 ox::Suspended

[`#include <ox/layout.hpp>`](../include/ox/layout.hpp)

```cpp
template <WidgetDerived ChildType>
class Suspended : public Widget;
```

A layout holding a single Widget centered within the available area. Its requested
size is clamped to the available dimensions.

```cpp
auto sus = Suspended{
    TextBox{{
        .style = {.background = colors::bright_black},
        .focus_policy = FocusPolicy::Strong,
    }},
    {.width = 40, .height = 5},
};
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
explicit Suspended(ChildType&& child, Area requested, Glyph fill = {U' '});
```

The constructor takes a child, a requested size, and a fill glyph for the surrounding
margins. Change `requested_size` and call `refresh()` to update its dimensions.
Refresh validates before changing child geometry and notifies the child even when
geometry is unchanged. Painting composes `fill_outside(canvas, child.bounds(), fill_glyph)`.

### Public Objects

```cpp
ChildType child;
Area requested_size;
Glyph fill_glyph;
```

---

### `get_child` Free Function

```cpp
template <WidgetDerived ChildType>
auto get_child(Suspended<ChildType>& sus) -> auto&
```

Returns the child Widget of the Suspended layout. This is provided for convenience and
uniformity with other Layouts.

---

</details>

## 🧩 ox::LineEdit

[`#include <ox/lineedit.hpp>`](../include/ox/lineedit.hpp)

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

See [text editing](text-editing.md#widget-integration-and-api-changes) for the shared
LineEdit/TextBox mutation commands, signals, validation, and viewport API.

## 🧩 ox::PixelGrid

[`#include <ox/pixelgrid.hpp>`](../include/ox/pixelgrid.hpp)

`class PixelGrid : public Widget;`

An owned row-major RGB image. Each terminal cell holds two pixels using `▄`:
upper pixel as background, lower pixel as foreground. Odd final rows use the
public `odd_pixel` color (black by default). Native cell size is
`pixel_cell_size(pixel_size())`; positioning and clipping use ordinary Canvas views.

```cpp
auto grid = ox::PixelGrid{ox::Area{10, 11}, ox::RGB{0}};
grid.set_pixel({0, 0}, ox::RGB{0xff0000}); // Requests repaint when changed.
grid.reset({20, 12}, ox::RGB{0x123456});
grid.set_image({2, 1}, {ox::RGB{0xff0000}, ox::RGB{0x0000ff}});
```

`pixel_size()` and `pixels()` expose dimensions and mutable/const spans.
`at(Point)` provides checked access; no-argument `at()` retains Widget geometry.
Direct span/reference edits and changes to `odd_pixel` require an application
repaint request. Image replacement invalidates spans and references. Moves transfer
Widget identity and image storage and leave an empty, reusable source.

Dimensions must be nonnegative; storage must contain exactly `width * height`
RGB values. Counts use checked arithmetic and reject sizes beyond the addressable
byte range. Invalid dimensions/storage throw `std::invalid_argument`, impossible
counts throw `std::length_error`, and invalid coordinates throw `std::out_of_range`.
`reset` and `set_image` preserve the current image if validation/allocation fails.

Custom widgets can compose the same drawing without owning a PixelGrid:

```cpp
void draw(ox::Canvas canvas, ox::Area size, std::span<ox::RGB const> pixels)
{
    ox::paint_pixels(canvas, size, pixels);
}
```

The painter validates storage before writing, visits only visible cells, and
leaves margins unchanged. Visible output requires a one-column lower-half block
under the Canvas width policy. `pixel_count` and `pixel_index` provide checked
row-major arithmetic for callers using their own vectors. The former Matrix and
public buffer API have been removed.

See the [`bmp_view.cpp`](../examples/bmp_view.cpp) example. Its decoder is kept
in the example and supports uncompressed 1/4/8/16/24/32-bit BMPs and 16/32-bit
bitfields, including top-down images, with bounds-checked headers, rows and palettes.

## 🧩 ox::ScrollBar

A vertical or horizontal scroll control with content_length, viewport_length,
position, style, and optional scroll_settle_time animation. set_position notifies
only on change; set_range synchronizes without feedback. Shared pure functions in
`<ox/scroll.hpp>` compute limits, movement, reveal ranges, and thumb/click geometry.

See [data tables and scrolling](data-tables.md#shared-scrolling-functions).

## 🧩 ox::TextBox

[`#include <ox/textbox.hpp>`](../include/ox/textbox.hpp)

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
See [text editing](text-editing.md#widget-integration-and-api-changes).

## 🧩 ox::Padding, ox::Stack, ox::ScrollView

`<ox/container.hpp>` supplies an inset child with background painting, a selected-page
container, and a clipped scrolling child. See [container composition](containers.md).

## 📦 ox::Point

[`#include <ox/core/terminal.hpp>`](../include/ox/core/terminal.hpp)

`struct Point;`

An `x` (horizontal) and `y` (vertical) coordinate on a 2D grid.

```cpp
auto p = Point{.x = 5, .y = 8};
```

<details>
<summary><strong>Details</strong></summary>

```cpp
struct Point {
    int x;
    int y;

    constexpr auto operator==(Point const& other) const -> bool = default;
    constexpr auto operator!=(Point const& other) const -> bool = default;
    constexpr auto operator+(Point const& other) const -> Point;
    constexpr auto operator-(Point const& other) const -> Point;
    constexpr auto operator<(Point const& other) const -> bool;
};
```

`operator+` and `operator-` operate on each corresponding element. ie, `x` is added to
`x` and `y` is added to `y`.

---

</details>

## 📦 ox::Area

[`#include <ox/core/terminal.hpp>`](../include/ox/core/terminal.hpp)

`struct Area;`

Holds `width` and `height` for a 2D grid.

```cpp
auto a = Area{.width = 4, .height = 7};
```

<details>
<summary><strong>Details</strong></summary>

```cpp
struct Area {
    int width;
    int height;

    constexpr auto operator==(Area const& other) const -> bool = default;
    constexpr auto operator!=(Area const& other) const -> bool = default;
};
```

---

</details>

## ox::Trait and ox::Traits

[`#include <ox/core/glyph.hpp>`](../include/ox/core/glyph.hpp)

These are Escape's validated text appearance attributes. `Trait` includes `Bold`,
`Dim`, `Italic`, `Underline`, `Blink`, `Inverse`, `Invisible`, `CrossedOut`, and
`DoubleUnderline`. `Standout` combines bold and inverse; `None` is empty.
`Traits` combines attributes and provides `contains`, `insert`, and `remove`.

```cpp
auto traits = Trait::Bold | Trait::Inverse;
auto glyph = U'x' | traits;
glyph = glyph | esc::without(Trait::Bold);
```

## 🧩 ox::Color and ox::Style

These types come directly from Escape:

```cpp
using Color = std::variant<esc::DefaultColor, IndexedColor, RGB>;

struct Style {
    Color background = default_color;
    Color foreground = default_color;
    Traits traits = {};
};
```

Use `colors::blue` and other named palette colors, `IndexedColor{235}` for a palette
slot, `RGB{0x773283}` for true color, or `RGB{HSL{120, 90, 70}}` for an HSL conversion.
`fg(color)` and `bg(color)` select a color channel. Widgets expose `style`,
`style`, `placeholder_style`, or `focused_style` as appropriate.

## 🧩 ox::Glyph and ox::GlyphString

```cpp
struct Glyph {
    char32_t symbol = U' ';
    Style style = {};
};
using GlyphString = std::vector<Glyph>;
```

A glyph is a styled Unicode scalar. Frames group scalars into complete graphemes and reserve their terminal column
footprints, including continuation cells for wide graphemes.
Use Escape's public conversion and glyph functions for validated text:

```cpp
auto label = esc::glyphs("Hello") | Trait::Bold | fg(colors::cyan);
auto literal = U"Hello"_gs;
auto scalar = U'•' | fg(colors::blue);
put(canvas, {0, 0}, label);
```

`GlyphRange` is Escape's concept for ranges of glyphs. Styling a glyph range returns
a glyph string. Styling plain text with Escape's `operator|` creates a composed
output value; use `esc::glyphs(text)` first when painting to a TermOx canvas.

---

## 🧩 ox::Terminal

[`#include <ox/core/terminal.hpp>`](../include/ox/core/terminal.hpp)

`class Terminal;`

Represents the terminal itself, providing an event loop and screen writing tools.
Application opens a terminal in `run(Terminal::Options)` on the calling thread.

```cpp
auto t = Terminal{{
    .mouse_mode = MouseMode::Move,
    .foreground = RGB{0x773283},
    .background = RGB{0xFF8A4A},
}};
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
struct Options {
    MouseMode mouse_mode = MouseMode::Basic;
    Signals signals = Signals::Off;
    Color foreground = default_color;
    Color background = default_color;
};

Terminal(Options options);

Terminal(MouseMode mouse_mode = MouseMode::Basic,
         Signals signals = Signals::Off,
         Color foreground_ = default_color,
         Color background_ = default_color);
```

Initialize the terminal screen to the 'alternate screen buffer' for interactive mode
with the given settings.

`MouseMode` determines which mouse events are generated:

```cpp
/**
 * Input modes for the Mouse.
 * @details Off: No Mouse Events are generated.
 * @details Basic: Mouse Press & Release Events for all Buttons + Scroll Wheel.
 * @details Drag: Basic, plus Move Events while a Button is pressed.
 * @details Move: Basic, plus Move Events sent with or without a Button pressed.
 */
enum class MouseMode { Off, Basic, Drag, Move };
```

`Signals` determines whether the OS generates signals from control keys. The default
is `Off`; TermOx handles Ctrl-C as an interrupt event and restores the terminal on exit.
With `On`, applications are responsible for OS signal handling:

```cpp
/**
 * `On` will generate signals from ctrl-[key] presses; `Off` will not.
 */
enum class Signals : bool { On, Off };
```

`foreground_` and `background_` determine the default colors for the Terminal.

```cpp
Terminal(Terminal const&) = delete;
Terminal(Terminal&&) = delete;
auto operator=(Terminal const&) -> Terminal& = delete;
auto operator=(Terminal&&) -> Terminal& = delete;
```

Terminal objects own an Escape session. Reading and rendering run on the calling thread.

---

### 🧹 Destructor

Restores the session-owned terminal state.

---

### Reading and rendering

`read(timeout)` returns an optional TermOx event. `commit_changes(screen, cursor)`
compares a supplied screen with the previous frame and presents its changes.
`size()` returns terminal dimensions. Frame storage and repaint requests belong to
`EventLoop`, described in [the event loop guide](event-loop.md).

</details>

## 🧩 ox::Frame and ox::Canvas

[`#include <ox/core/frame.hpp>`](../include/ox/core/frame.hpp)
[`#include <ox/core/canvas.hpp>`](../include/ox/core/canvas.hpp)

`Frame` owns cells containing complete graphemes, styles, and column widths.
Continuation cells mark the rest of a wide glyph's footprint. Cell access is read-only;
use `write()` or the drawing functions to preserve footprint invariants.

`Canvas` borrows a frame and exposes local coordinates, inherited clipping, and
`subcanvas(offset, size)`. All drawing operations clip without changing the requested
geometry. Resizing resets the frame; existing canvas objects safely intersect their
retained geometry with the new bounds.

```cpp
auto frame = Frame{{20, 5}};
auto canvas = Canvas{frame};
auto panel = canvas.subcanvas({2, 1}, {10, 3});
put(panel, {0, 0}, "Hello 界");
```

See [the frame and canvas guide](frame-and-canvas.md) for coordinate checks, resize,
Unicode width policies, complete-footprint clipping, and borrowing rules.

## 🧩 Layout calculation

[`#include <ox/core/layout.hpp>`](../include/ox/core/layout.hpp)

`distribute_length(policies, available)` returns integer lengths from a span of
`SizePolicy` values. `layout_row(policies, area)` and `layout_column(policies, area)`
return consecutive rectangles. `centered_rect(available, requested)` returns a clamped,
centered rectangle. These functions have no widget or terminal dependencies.

See [layout calculation](layout.md) for allocation rules and edge cases.

## 🧩 Text editing

[`#include <ox/core/text.hpp>`](../include/ox/core/text.hpp)

`TextEdit` owns UTF-8 text, grapheme-boundary carets, selection, bounded undo/redo,
and atomic paste assembly. `TextLayout` owns cached display rows and width assumptions;
`layout_text()` is the pure calculation. Byte offsets and terminal columns remain
explicitly separate. LineEdit and TextBox compose these with the functions in `<ox/text_view.hpp>`.

See [text editing](text-editing.md) for normalization, navigation, history, rendering,
and the new widget text accessors.

## 🧩 ox::Renderer

[`#include <ox/core/renderer.hpp>`](../include/ox/core/renderer.hpp)

`Renderer(profile)` owns an Escape capability profile and reusable presentation storage.
`render(frame, cursor, foreground, background)` returns borrowed encoded bytes without
terminal I/O. Adjacent changes form runs; unchanged frames produce no bytes. Cursor
position and effective style are retained across calls. The caller writes the bytes
and manages visibility and flushing. Call `invalidate()` if the bytes were discarded,
output failed, or another writer changed terminal state.

See [the renderer guide](renderer.md) for buffer lifetime, Unicode boundaries,
capability adaptation, resize behavior, and failure recovery.

## 🧩 Routing and focus

[`#include <ox/routing.hpp>`](../include/ox/routing.hpp)

`widget_tree(root)` returns a snapshot with ancestry, clipped bounds, lifetime views,
and input eligibility. `hit_path(tree, point)` returns mouse ancestry;
`focus_order(tree)` returns visible Tab/Strong candidates. `Focus(root, loop)` owns
selection for one tree. Widget input, hover, focus, and timer callbacks return
`EventResponse`; mouse and keyboard input bubble until handled or quitting.

See [routing and focus](routing-and-focus.md) for ownership, hover transitions,
Tab behavior, callback mutations, and the breaking callback API change.

## 🧩 ox::EventLoop

[`#include <ox/core/event_loop.hpp>`](../include/ox/core/event_loop.hpp)

Owns queued events, scheduled timers, frame storage, repaint state, and the exit code.
`post()` is thread safe. `step(handler, now)` and `paint(handler)` run without terminal
I/O. `wait(terminal.wait_state())` waits for terminal readiness, queued posts, or the
next deadline without consuming input. `wait()` can also wait without a terminal.
`process_events(loop, terminal, handler)` drives the same core with terminal input.

## 🔢 ox::Event

A `std::variant` of input event types. This is used by the core of the library and
direct access should not be needed by the typical user of this library.

```cpp
using Event = std::variant<esc::MousePress,
                           esc::MouseRelease,
                           esc::MouseWheel,
                           esc::MouseMove,
                           esc::KeyEvent,
                           esc::Focus,
                           esc::PasteChunk,
                           esc::UnknownInput,
                           esc::TerminalReply,
                           esc::Resize,
                           event::Custom,
                           event::Interrupt>;
```
## Selection and application widgets

`ListView`, `Select`, `ComboBox`, `Tabs`, `Popup`, `Dialog`, `Menu`, and `MenuBar` compose
shared row selection, clipping, text editing, containers, and routing. See the
[selection and application guide](selection-and-application.md) for commands, signal
contracts, keyboard/mouse bindings, lifetime rules, and examples.

See [values and composite widgets](value-and-composite-widgets.md) for Slider,
NumericEdit, ProgressBar, Spinner, Switch, TreeView, LogView, SplitPane, Form,
FilePicker, and CommandPalette.


### Typed numeric input and shared scrolling

`NumericEdit` is `BasicNumericEdit<double>`; `IntegerEdit` is
`BasicNumericEdit<int>`. Bounds, steps, values, and `on_change` signals use the numeric
type. Configure `.format`, `.prefix`, `.suffix`, `.locale`, and
`.commit_on_focus_loss` without changing the simple default construction. See
[values and composite widgets](value-and-composite-widgets.md) for precision-safe
editing and form commit behavior.

All standard scrollable views implement `Scrollable` and use the same
`link(view, scrollbar)` function. RadioGroup now takes `Point{0, row}` offsets;
TreeView and LogView expose scrolling directly. See
[shared scrolling](containers.md#shared-scrolling-interface).

See [shared commands](commands.md) for commands reused by menus, palettes, shortcuts, and KeyHelp.

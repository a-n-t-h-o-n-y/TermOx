# API Reference

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
thread. `focus()` returns the application's focus manager.

See [the event loop guide](event-loop.md) for the response contract and scheduling.

## 🧩 ox::Timer

[`#include <ox/timer.hpp>`](../include/ox/timer.hpp)

A movable timer handle using the event loop's steady-clock scheduler. Timers create no
threads. Retain the handle to retain its schedule.

```cpp
Timer(Widget& widget, std::chrono::milliseconds interval, bool launch = false);
Timer(std::chrono::milliseconds interval,
      std::function<EventResponse()> callback,
      bool launch = true);
```

Widget timers are adopted by the owning application. Register a callback timer with
`loop.watch(timer)`. Intervals must be positive. `start(now)` resets the deadline,
`stop()` cancels it, and `is_running()` reports whether a deadline exists. Delayed
execution produces one tick, preserving cadence and skipping missed ticks.

## 🧩 ox::Focus

[`#include <ox/focus.hpp>`](../include/ox/focus.hpp)

A focus manager owned by each application. `app.focus().set(widget)` sends focus-out
and focus-in callbacks, `get()` returns a lifetime view, and `clear()` removes focus.
`Widget::focused` exposes the current focus state to painting code.

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
void paint(Canvas c) override {
    put(c, {.x = 5, .y = 10}, 'X');
    put(c, {.x = 6, .y = 10}, 'X' | fg(colors::blue));
    put(c, {.x = 7, .y = 10}, "Hello!");

    put(c, {.x = 0, .y = 0}, shape::HLine{}, this->size.width);
    put(c, {.x = 0, .y = 0}, shape::VLine{}, 3, Style{.background = RGB{0x337283}});
    put(c, {.x = 0, .y = 1}, shape::Box::dashed(), {.width = 10, .height = 4});

    put(c, shape::Box::bold());  // Outlines the Canvas.
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
void paint(Canvas c) override {
    fill(c, 'X');
    fill(c, fg(RGB{0x33A421}));
    fill(c, Trait::Bold | Trait::Standout);

    auto sub_region = Canvas{
        .buffer = c.buffer,
        .at = c.at + {.x = 3, .y = 0}, 
        .size = {.width = 5, .height = 7},
    };
    fill(sub_region, 'X' | bg(colors::red) | fg(colors::black));
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
void paint(Canvas c) override {
    fill(c, fg(RGB{0x33A421}));
    clear(c);
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

`class Widget;`

The user input and graphical display unit. Each Widget has a particular piece of the terminal display where is recieves user input events and paints to the screen.

User input event handlers are virtual functions that are overridden to make state
changes; the paint event handler allows output of that state onto the screen and is always automatically called after an event is sent to the Widget.

```cpp
class MyWidget : public Widget {
   public:
    MyWidget() : Widget{FocusPolicy::None, SizePolicy::flex()} {}

    void paint(Canvas c) override { 
        for (auto const at : points_) {
            put(c, at, U'•' | fg(colors::red));
        }
    }

    auto mouse_press(MousePress m) -> EventResponse override
    {
        if (m.button != MouseButton::Left) { return {}; }
        auto const inserted = points_.emplace(m.at).second;
        return {.handled = true, .repaint = inserted};
    }

   private:
    std::set<Point> points_;
};
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
Widget(FocusPolicy fp, SizePolicy sp);
```

There are no defaults for the constructor parameters, they must be explicitly given by
each derived class.

```cpp
Widget(Widget const&) = delete;
Widget(Widget&& other);

auto operator=(Widget const&) -> Widget& = delete;
auto operator=(Widget&& other) -> Widget&;
```

Move only type. The move constructor/assignment operator will move the lifetime of this
Widget into the source of the move.

---

### 🧹 Destructor

```cpp
virtual ~Widget() = default;
```

Does nothing special, the `Widget::lifetime` object will be destroyed (if it hasn't
already been moved from), and anything that depends on that lifetime will react
appropriately.

---

### Public Objects

```cpp
FocusPolicy focus_policy;
SizePolicy size_policy;
Cursor cursor = std::nullopt;
bool active = true;
bool enabled = true;

Point at = {.x = 0, .y = 0};
Area size = {.width = 0, .height = 0};
std::shared_ptr<Widget*> lifetime = std::make_shared<Widget*>(this);
```

Any of these can be updated by event handlers, and any changes will be immediately
visible on screen. The author recommends not messing with `at` and `size` unless you are
the parent Widget and are implementing a layout type. And obviously `lifetime` is a
'look, don't touch' object, unless you are feeling particularly adventurous.

---

### `Widget::mouse_press`

```cpp
virtual auto mouse_press(MousePress) -> EventResponse;
```

Event handler for receiving mouse press events that occur within the bounds of this
Widget. If the Widget is a layout, it will not recieve mouse press events intended for
its children.

Events from `Button::ScrollUp` and `Button::ScrollDown` are not received here, see
`Widget::mouse_wheel`.

---

### `Widget::mouse_release`

```cpp
virtual auto mouse_release(MouseRelease) -> EventResponse;
```

Event handler for receiving mouse release events that occur within the bounds of this
Widget. If the Widget is a layout, it will not recieve mouse press events intended for
its children.

---

### `Widget::mouse_wheel`

```cpp
virtual auto mouse_wheel(MouseWheel) -> EventResponse;
```

Event handler for receiving events generated by either `Button::ScrollUp` or
`Button::ScrollDown`. Each 'click' or increment of the mouse wheel generates a single
event.

---

### `Widget::mouse_move`

```cpp
virtual auto mouse_move(MouseMove) -> EventResponse;
```

Event handler for events generated by movement of the mouse over the Widget. This has to
be enabled in the `Terminal` constructor with the correct `MouseMode` option, either
`MouseMode::Move` or `MouseMode::Drag`.

---

### `Widget::mouse_enter`

```cpp
virtual auto mouse_enter() -> EventResponse;
```

Event handler for events generated by movement of the mouse into the Widget from another
Widget. This has to be enabled in the `Terminal` constructor with the correct
`MouseMode` option, either `MouseMode::Move` or `MouseMode::Drag`.

---

### `Widget::mouse_leave`

```cpp
virtual auto mouse_leave() -> EventResponse;
```

Event handler for events generated by movement of the mouse out of the Widget and into
another Widget. This has to be enabled in the `Terminal` constructor with the correct
`MouseMode` option, either `MouseMode::Move` or `MouseMode::Drag`.

---

### `Widget::focus_in`

```cpp
virtual auto focus_in() -> EventResponse;
```

Called when this Widget recieves keyboard focus. The Widget's `focus_policy` must not be
`FocusPolicy::None` to receive focus.

---

### `Widget::focus_out`

```cpp
virtual auto focus_out() -> EventResponse;
```

Called when this Widget gives up keyboard focus. The Widget's `focus_policy` must not be
`FocusPolicy::None` to receive focus.

---

### `Widget::key_press`

```cpp
virtual auto key_press(KeyEvent) -> EventResponse;
```

Called on keyboard key presses and repeats when this Widget has focus. See Escape for
what types of keypresses are generated.

---

### `Widget::key_release`

```cpp
virtual auto key_release(KeyEvent) -> EventResponse;
```

Called on keyboard key release when this Widget has focus. Release reporting depends on
Terminal in order to receive these events, its support is experimental and as the author
I'd recommend against using it for serious projects.

---

### `Widget::resize`

```cpp
virtual void resize(Area old_size);
```

Called each time the Widget receives a new size. Note that the parameter to this
function is the previous size of the Widget, the Widget will already have its `size`
member updated to the new size.

---

### `Widget::timer`

```cpp
virtual auto timer() -> EventResponse;
```

Called after each passing of the time interval of an associated `Timer` object. Each
Timer object that is currently running and was constructed with the current Widget will
cause this to be invoked.

---

### `Widget::paint`

```cpp
virtual void paint(Canvas);
```

Called whenever the Widget needs a repaint, in reality it is called once for all
currently enabled Widgets at the end of each event loop iteration. This function never
needs to be manually called, the Application instance will handle this.

The Canvas passed to this function is already sized to the Widget.

---

### `Widget::get_children`

```cpp
virtual auto get_children() -> zzz::Generator<Widget&>;
virtual auto get_children() const -> zzz::Generator<Widget const&>;
```

Return a coroutine generator to each child of this Widget. This only needs to be
implemented if you are creating a new layout type that owns child Widgets.

---

</details>

## 📦 ox::Connection

[`#include <ox/widget.hpp>`](../include/ox/widget.hpp)

```cpp
template <typename SignalType, typename SlotType>
struct Connection;
```

This struct is provided to make connecting `sl::Signal`s and `sl::Slot`s clearer.

```cpp
// Increment the total_label's value each time the numberpad is pressed.
Connection{
    .signal = numberpad.on_press,
    .slot = [](int n, auto& total_label) { total_label.value += n; },
}(total_label);

// numberpad.on_press is a sl::Signal<void(int)>, passing on the value to increment by.

// The slot is a function that takes that passed in value to increment by and a
// reference to the totals label.

// The total_label object has its lifetime tracked by passing it to Connection::op(),
// which will make sure the Slot receives it as a parameter if the object is still valid
```

<details>
<summary><strong>Details</strong></summary>

TermOx uses the [SignalsLight](https://github.com/a-n-t-h-o-n-y/signals-light) Library
to provide a Signal/Slot mechanism. A Signal is anything a Widget wants to 'emit' or let
others know about. For example the `Button` Widget provides a `sl::Signal<void()>
on_press` signal that is 'emitted' each time the left mouse button is pressed. Any other
Widget that has access to the Button can then connect a function (Slot) to this Signal
to be invoked whenever that button is pressed. Because signals are being sent between
Widgets, you might want to make sure a Slot does not reference a dangling Widget
reference, so lifetime tracking is provided. This struct attempts to package all of that
into a single call site.

This struct allows for designated initilizer syntax so the `signal` and `slot` members
can be explicitly identified in code.

The `operator()` overload provides a way to track Widget lifetimes, Slots tend to need
to capture Widgets to operate on, if references are stored in lambda captures, those
references may be left dangling when a Widget is moved from. With `operator()`, you get
lifetime tracking and the tracked widgets are passed into the connected Slot as function
parameters if they are valid, otherwise the Slot is not invoked.

```cpp
template <typename SignalType, typename SlotType>
struct Connection {
    SignalType& signal;
    SlotType slot;

    template <typename... TrackedWidgets>
    auto operator()(TrackedWidgets&... to_track) && -> sl::Identifier;
};
```

---

### Connection::operator()

```cpp
template <typename... TrackedWidgets>
auto Connection::operator()(TrackedWidgets&... to_track) && -> sl::Identifier;
```

Make the connection and pass any tracked Widgets into the connection. If any of these
tracked Widgets is not valid, the Slot will not be invoked. If they are all valid, the
Slot will be invoked with the parameters from the Signal first, then the tracked Widgets
after.

Must be called to actually make the connection of the Slot to the Signal even if nothing
is being tracked. A warning should be issued by the compiler if this is not called.

The returned `sl::Identifier` can be used to disconnect the Slot from the Signal. Any
number of Slots can be connected to the same Signal.

---

</details>

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

```cpp
template <typename ConcreteType>
class LifetimeView;
```

Provides the ability to track a Widget's lifetime. This should be used with the
corresponding `track(...)` function to construct a LifetimeView. LifetimeView is useful
for Signals/Slots to check if any captured Widgets are no longer around.

```cpp
auto my_widg = Widget{...};
auto lv = track(my_widg);
auto widg2 = std::move(my_widg); // lv will now point here.
if (lv.valid()) { my_widg.get().do_thing(); }
```

<details>
<summary><strong>Details</strong></summary>

It does this by accessing the `lifetime` shared pointer within the Widget object and
storing a weak pointer to it. The Widget class also updates the shared pointer on move
operations. It keeps track of the concrete type of the Widget, so it is able to return a
reference type that is actually useful.

### 🏗️ Constructors

```cpp
LifetimeView(std::shared_ptr<Widget*> wp);
```

Use `track(my_widg);` instead of the constructor.

The type is Copyable and Movable.

---

### `LifetimeView::valid`

```cpp
auto valid() const -> bool;
```

Returns `true` if the tracked Widget is alive.

---

### `LifetimeView::get`

```cpp
auto get() const -> ConcreteType&;
```

Returns a reference to the tracked Widget, cast to the ConcreteType provided at
construction. Undefined behavior if the tracked Widget is not valid.

---

</details>

## 🔢 ox::Align

[`#include <ox/align.hpp>`](../include/ox/align.hpp)

```cpp
enum class Align;
enum class VAlign;
```

Describes horizontal (`Align`) or vertical (`VAlign`) alignment of text. Used by various
Widgets.

```cpp
Label{{.text = "Hello", .align = Align::Center}};
```

<details>
<summary><strong>Details</strong></summary>

```cpp
enum class Align : std::uint8_t { Left, Center, Right };

enum class VAlign : std::uint8_t { Top, Middle, Bottom };
```
---

</details>

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

TermOx uses Escape's `KeyEvent` directly for `key_press` and `key_release`.
`Key` is a variant of `char32_t`, `KeyCode`, and `esc::KittyKey`. An event has
an optional key identity, `Modifiers`, a `KeyAction`, and associated UTF-32 text.
An IME event can carry text without a key identity.

```cpp
auto key_press(KeyEvent event) -> EventResponse override
{
    if (event.matches(KeyCode::Enter, {}, event.action)) {
        // Accept the current selection.
        return {.handled = true, .repaint = true};
    }
    if (event.matches(U'q', {}, event.action)) { return {.handled = true, .quit = 0}; }
    return {};
}
```

Press and repeat actions go to `key_press`; release actions go to `key_release`.
`input_text(event)` returns printable text for editing, excluding releases and
control shortcuts. `paste(esc::PasteChunk const&)` receives bracketed paste chunks.
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
    Label label = {};
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

Wraps a Widget in a Border. This is itself a Widget and will have 2 more units of space
in each dimension than the wrapped Widget, to account for Border space.

This is technically a layout, the wrapped Widget is accessed via the `Bordered::child` member.

A Widget can be easily wrapped in a Border by using the `operator|` overload:

```cpp
template <WidgetDerived WidgetType>
auto operator|(WidgetType w, Border b) -> Bordered<WidgetType>;
```

The `operator|` overload will create a new Bordered object that will take ownership of
the passed in Widget.

```cpp
    auto bordered = MyWidget{} | Border{.box = shape::Box::round(), .label = "A Label"};
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
    static constexpr auto ascii() -> Lines { return {U'-', U'|'}; }
    static constexpr auto empty() -> Lines { return {U' ', U' '}; }
};
```

## 🧩 ox::Divider

[`#include <ox/bordered.hpp>`](../include/ox/bordered.hpp)

`class Divider : public Widget;`

A single line Widget for dividing Widgets within a layout. Vertical/Horizontal is
determined automatically based on which layout it is used within.

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
    Lines lines = Lines::light();
    Style style = {};
    Label label = {};
};

Divider(Options x);
```

The `Divider::style` member only applies to the Line, and not the Label, The Label has
its own internal Style.

---

### Public Objects

```cpp
Lines lines;
Style style;
Label label;
```

---

</details>

## 🧩 ox::BasicButton

[`#include <ox/button.hpp>`](../include/ox/button.hpp)

`class BasicButton : public Widget;`

A simple button Widget, has no special visual elements, use `Button` if you want more.
Emits Signals on left mouse button press and release and enter key press.

```cpp
int i = 0;
auto btn = BasicButton{};
Connection{
    .signal = btn.on_press,
    .slot = [&] { ++i; },
}();
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
BasicButton(FocusPolicy fp = FocusPolicy::Strong,
            SizePolicy sp = SizePolicy::flex());
```

---

### Signals

```cpp
sl::Signal<void()> on_press;
sl::Signal<void()> on_release;
```

- `on_press` emitted with left mouse button press and enter key press.
- `on_release` emitted with left mouse button release.

---

</details>

## 🧩 ox::Button

[`#include <ox/button.hpp>`](../include/ox/button.hpp)

`class Button : public BasicButton;`

A button with an optional Label and decoration. The decoration can be permanent or
appear only on mouse hover, with animated fade in and out.

```cpp
int i = 0;
auto btn = Button{{
    .label = {.text = "A Button"},
    .decoration = Fade{
        .paint_fn = shape_gradient(shape::Box::brackets_round(),
                                   RGB{0x0a3f46}, RGB{0x1ecbe1}),
    },
    .press_mod = [](Label& l) { l.style.foreground = colors::bright_white; },
    .focus_mod = [](Label& l) { l.style.foreground = colors::bright_cyan; },
}};
Connection{
    .signal = btn.on_press,
    .slot = [&] { ++i; },
}();
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
struct Fade {
    /// (Canvas, percent) where percent is fade amount from [0, 1].
    std::function<void(Canvas, float)> paint_fn;
    std::chrono::milliseconds fade_in = std::chrono::milliseconds{200};
    std::chrono::milliseconds fade_out = std::chrono::milliseconds{400};
};

using PaintFn = std::function<void(Canvas)>;
using Decoration = std::variant<PaintFn, Fade>;

struct Options {
    Label::Options label = {};
    Decoration decoration = [](Canvas) {};
    std::function<void(Label&)> press_mod = [](Label&) {};
    std::function<void(Label&)> focus_mod = [](Label&) {};
    FocusPolicy focus_policy = FocusPolicy::Strong;
    SizePolicy size_policy = SizePolicy::flex();
};

Button(Options x = {});
```

Construct a Button Widget. The label recieves the entire Canvas to paint on, in practice
this means it will be centered vertically and its Align parameter will determine its
horizontal alignment.

The decoration takes either a simple paint function, or a Fade object. If a `PaintFn` is
used, it will be handed a Canvas that has the Label already painted to it, any
changes to this Canvas will appear on top of the Button. If a `Fade` object is given,
the button will have a fade in effect on `mouse_enter` and fade out on `mouse_leave`.
This fade effect is controlled by `Fade::paint_fn`, which takes a Canvas and float
percent (0 to 1, where 0 is fully off and 1 is fully on).

The `press_mod` function will be called for each paint event where the button has been
pressed but not yet released (it is not activated by an enter key press). The label is
passed into this function rather than the entire Canvas.

The `focus_mod` function will be called for each paint event where the button has focus.
The label is passed into this function rather than the entire Canvas.

---

### Public Objects

```cpp
Label::Options label;
std::function<void(Label&)> press_mod;
std::function<void(Label&)> focus_mod;
```

---

### `shape_gradient` Free Function

```cpp
auto shape_gradient(auto const& shape, RGB one, RGB two)
    -> std::function<void(Canvas, float)>
```

Builds a `paint_fn` for `Fade` (`void(Canvas, float)`) from a Shape object and gradient.
Shape must be able to be used with put(Canvas, shape, fg). The returned function will
paint the shape onto the Canvas, its foreground color will be `one` when the percent is
`0`, and `two` when the percent is `1`, and somewhere inbetween for other values.

---

</details>

## 🧩 ox::CheckBox

[`#include <ox/checkbox.hpp>`](../include/ox/checkbox.hpp)

`class CheckBox : public Widget;`

A simple checkbox Widget with two states, `Checked` and `UnChecked`. This does not
contain a Label, only a box.

```cpp
Column{
    CheckBox{}, // default is UnChecked
    CheckBox{{.state = CheckBox::Checked}},
    CheckBox{{.state = CheckBox::UnChecked}},
};
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
enum class State : bool { Checked, UnChecked };

struct Display {
    std::vector<Glyph> checked;
    std::vector<Glyph> unchecked;
    Style in_focus_style;
};

struct Options {
    State state = State::UnChecked;
    Display display = {
        .checked = U"[X]"_gs,
        .unchecked = U"[ ]"_gs,
        .in_focus_style = {.traits = Trait::Bold},
    };
    FocusPolicy focus_policy = FocusPolicy::Strong;
};

CheckBox(Options x = {});
```

Construct a CheckBox Widget. `state` is the initial state of the CheckBox.

The display object describes the look of each state in Glyphs and the Style to use when
the CheckBox is in focus.

---

### Signals

```cpp
sl::Signal<void()> on_check;
sl::Signal<void()> on_uncheck;
```

- `on_check` is emitted any time the state is changed from UnChecked to Checked.
- `on_uncheck` is emitted any time the state is changed from Checked to UnChecked.

---

### Public Objects

```cpp
Display display;
```

The current display object.

---

### `CheckBox::toggle`

```cpp
void toggle();
```

Change the state from `UnChecked` to `Checked` or from `Checked` to `UnChecked`,
emitting the appropriate signal.

---

### `CheckBox::check`

```cpp
void check();
```

If state is `UnChecked`, sets the state to `Checked` and emits the `on_check` signal.

---

### `CheckBox::uncheck`

```cpp
void uncheck();
```

If state is `Checked`, sets the state to `UnChecked` and emits the `on_uncheck` signal.

---

### `CheckBox::set_state`

```cpp
void set_state(State s);
```

Set the current state to `s`, emit the appropriate signal if the state changes.
If the state does not change, no signal is emitted.

---

### `CheckBox::get_state`

```cpp
auto get_state() const -> State;
```

Get the current state of the CheckBox.

---

</details>

## 🧩 ox::DataTable

[`#include <ox/datatable.hpp>`](../include/ox/datatable.hpp)

`class DataTable : public Widget;`

A 2D table of strings, with a header for each column. This is not intended to be a full
spreadsheet application, it is intended to be a simple way to display data in a grid
with dividing lines.

```cpp
auto dt = DataTable{};
dt.add_column("one", Align::Center, colors::blue);
dt.add_row({"1"});
dt.add_column("two", Align::Center);
dt.add_column("three", Align::Center);
dt.add_row({"2", "3", "4"});
dt.add_row({"5", "6", "7"});
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
struct Options {
    Color background = default_color;
    Color foreground_cell = default_color;
    Color foreground_line = default_color;
    SizePolicy size_policy = SizePolicy::flex();
};

DataTable(Options x = {});
```

---

### `DataTable::add_column`

```cpp
void add_column(std::string heading,
                Align align = Align::Left,
                Color foreground = default_color);
```

Add a column to the table. The heading is the text that will be displayed at the top of
the column. The alignment is the alignment of the text in the column. The foreground
color is the color of the text in the column heading. The column will be appear at the
right most position in the table.

---

### `DataTable::add_row`

```cpp
void add_row(std::vector<std::string> row);
```

Add a row to the table. The row is a vector of strings, each string is the text that
will be displayed in the cell. The row must have the same number of columns as the
table, otherwise a std::out_of_range exception will be thrown. The row will be added to
the bottom of the table.

---

### `link` Free Function

```cpp
void link(DataTable& dt, ScrollBar& sb);
```

Link a ScrollBar to a DataTable, the ScrollBar will control the DataTable, and the
DataTable will update the ScrollBar.

---

</details>

## 🧩 ox::Label

[`#include <ox/label.hpp>`](../include/ox/label.hpp)

`class Label : public Widget;`

A Widget displaying a static single line of text. The text is not editable, but it can be set directly via its public data members. The text is centered vertically, its horizontal alignment is determined by the `Align` constructor parameter.

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
    Align align = Align::Center;
    Style style = {};
    SizePolicy size_policy = SizePolicy::flex();
};

Label(Options x = {});

Label(std::string text_);

Label(char const* text_);
```

The `std::string` and `char const*` constructors are provided for convenience, they will
set the `text` member to the provided value and all other options to their default
values.

The `Options` constructor takes a `std::string` for the text, an `Align` enum for the
horizontal alignment, a `Style` to apply to each cell of the Label, and a `SizePolicy`.

---

### Public Objects

```cpp
std::string text;
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
    SizePolicy size_policy = SizePolicy::flex();
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
col2.children.push_back(Button{"A Button"} | SizePolicy::fixed(5));
col2.children.push_back(Button{"B Button"} | SizePolicy::flex());
col2.children.push_back(Button{"C Button"} | SizePolicy::fixed(5));
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
    Button{"A Button"} | SizePolicy::fixed(5),
    TextBox{} | SizePolicy::flex(),
    Button{"B Button"} | SizePolicy::fixed(5),
};

auto row2 = Row<std::vector<Button>>{};
row2.children.push_back(Button{"A Button"} | SizePolicy::fixed(5));
row2.children.push_back(Button{"B Button"} | SizePolicy::flex());
row2.children.push_back(Button{"C Button"} | SizePolicy::fixed(5));
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
margins. Change `requested_size` before the next layout to update its dimensions.

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

A user editable single line text input Widget. 

Note: Right and Center Align are not supported yet.

```cpp
auto line_edit = LineEdit{{
    .text = "A LineEdit",
    .text_style = {.foreground = colors::bright_white},
    .background = colors::bright_blue,
    .ghost_text = "Ghost Text",
    .validator = [](char c) { return std::isalnum(c); },
}};
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
struct Options {
    std::string text = "";
    Style text_style = {};
    Align align = Align::Left;
    Color background = default_color;
    std::string ghost_text = "";
    Style ghost_text_style = {.foreground = colors::bright_black};
    std::optional<std::function<bool(char32_t)>> validator = std::nullopt;
    FocusPolicy focus_policy = FocusPolicy::Strong;
    SizePolicy size_policy = SizePolicy::flex();
};

LineEdit(Options x = {});
```

Construct a LineEdit Widget. The `text` is the initial text to be displayed, the
`text_style` is the Style to use when painting the text, `align` is the horizontal
alignment of the text, `background` is the background color of the LineEdit,
`ghost_text` is the text to be displayed when the LineEdit is empty, `ghost_text_style`
is the Style to use when painting the ghost text, `validator` is a function that will be
called for each character typed into the LineEdit.

---

### Signals

```cpp
sl::Signal<void(std::string const&)> on_enter;
```

- `on_enter` is emitted when the user presses the enter key while the LineEdit has
focus. The current text is passed to the signal. This does not emit the ghost text.

---

### Public Objects

```cpp
TextEdit editor;
auto text() const -> std::string const&;
void set_text(std::string_view text);
Style text_style;
Align align;
StylePatch selection_style;
Color background;
std::string ghost_text;  // Visible if text is empty.
Style ghost_text_style;
std::optional<std::function<bool(char32_t)>> validator;
```

---

</details>

## 🧩 ox::PixelGrid

[`#include <ox/pixelgrid.hpp>`](../include/ox/pixelgrid.hpp)

`class PixelGrid : public Widget;`

A 2D grid of pixels, each pixel is a `RGB` object. Each cell on the terminal holds 2 'pixels', the `▄` character is used to do the drawing.

See the [`bmp_view.cpp`](../examples/bmp_view.cpp) example.

```cpp
auto grid = PixelGrid{Area{.width = 10, .height = 10}};
grid.buffer[{.x = 0, .y = 0}] = RGB{0xFF0000};
grid.buffer[{.x = 1, .y = 0}] = RGB{0x00FF00};
grid.buffer[{.x = 2, .y = 0}] = RGB{0x0000FF};
// ...
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
struct Matrix {
    Area area;
    std::vector<RGB> pixels;

    auto operator[](Point p) -> RGB&;
    auto operator[](Point p) const -> RGB;
};

PixelGrid(Area area);

PixelGrid(Matrix matrix);
```

The first constructor takes an `Area` object, which is the size of the grid.

The second constructor takes a `Matrix` object, which is a 2D array of `RGB`
objects. Build up a `Matrix` object with the `Matrix::operator[]` function, do not
modify the `pixels` vector directly.

---

### Public Objects

```cpp
Matrix buffer;
```

Direct access to the buffer of pixels.

---

</details>

## 🧩 ox::ScrollBar

[`#include <ox/scrollbar.hpp>`](../include/ox/scrollbar.hpp)

`class ScrollBar : public Widget;`

A vertical scroll bar Widget. It can be linked with various Widgets to provide
scrolling functionality.

```cpp
auto tb = TextBox{{
    .text = "A TextBox",
}};

auto sb = ScrollBar{};

link(tb, sb);
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
struct Options {
    int scrollable_length = 0;
    int position = 0;
    Style style = {};
    std::chrono::milliseconds scroll_settle_time = std::chrono::milliseconds{500};
};

ScrollBar(Options x = {});
```

Construct a ScrollBar Widget. The `scrollable_length` is the length of the scrollable
area, the `position` is the current position of the scroll bar, the `style` is the Style
to use when painting the scroll bar, and the `scroll_settle_time` is the time it takes
for the scroll bar to settle after a scroll event. The `position` is clamped to the `[0,
scrollable_length)` range.

Often you can just default construct a ScrollBar and use the `link` function to link it
to a Widget. The `link` function will set the `scrollable_length` and `position` to
the correct values for the linked Widget.

---

### Signals

```cpp
sl::Signal<void(int)> on_scroll;
```

- `on_scroll` is emitted on any change to `position`. The current position is passed to
the signal.

---

### Public Objects

```cpp
int scrollable_length;
int position;  // [0, scrollable_length)
Style style;
std::chrono::milliseconds scroll_settle_time;
```

---

</details>

## 🧩 ox::TextBox

[`#include <ox/textbox.hpp>`](../include/ox/textbox.hpp)

`class TextBox : public Widget;`

A basic TextBox Widget. It supports word wrapping and scrolling. It is editable if a
FocusPolicy other than `None` is set. It can be used with a ScrollBar with the `link`
function.

```cpp
auto tb = TextBox{{
    .text = "A TextBox",
    .wrap = TextWrap::Word,
    .align = Align::Center,
    .style = {.foreground = colors::bright_white, .traits = Trait::Bold},
}};

auto sb = ScrollBar{};

link(tb, sb);
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
enum class TextWrap { None, Any, Word };

struct Options {
    std::string text = {};  // UTF8 encoded
    TextWrap wrap = TextWrap::Word;
    Align align = Align::Left;
    Style style = {};
    FocusPolicy focus_policy = FocusPolicy::None;
    SizePolicy size_policy = SizePolicy::flex();
};

TextBox(Options x = {});
```

Construct a TextBox Widget. The `text` is the initial text to be displayed, the `wrap`
is the wrapping mode (`Any` wraps at grapheme boundaries, and `Word` prefers spaces
or tabs), `align` is the horizontal alignment of the text, `style` is the Style to
use when painting the text.

---

### Signals

```cpp
sl::Signal<void(int, int)> on_scroll; /// void(int index, int size)
```

- `on_scroll` is emitted on any scroll parameter update. The current index and size are
passed to the signal. This is used by the `link` function and does not need to be used
directly.


### Public Objects

```cpp
TextEdit editor;
auto text() const -> std::string const&;
void set_text(std::string_view text);
TextWrap wrap;
Align align;
Style style;
StylePatch selection_style;
```

---

### `link` Free Function

```cpp
void link(TextBox& tb, ScrollBar& sb);
```

Link a ScrollBar to a TextBox, the ScrollBar will control the TextBox, and the
TextBox will update the ScrollBar.

---

</details>

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
`text_style`, `ghost_text_style`, or `in_focus_style` as appropriate.

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
explicitly separate. LineEdit and TextBox compose these with `put_text_line()`.

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
I/O. `process_events(loop, terminal, handler)` drives the same core with terminal input.

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
                           event::Timer,
                           event::Custom,
                           event::Interrupt>;
```
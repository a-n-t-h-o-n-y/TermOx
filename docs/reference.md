# API Reference

## 🧩 ox::Application

[`#include <ox/application.hpp`](../include/ox/application.hpp)

`class Application;`

This is the main entry point to any **TermOx** Widget based application. The Application
class orchestrates reading of Events and writing updates to the Terminal. It implements
event handler functions to process Events.

```cpp
int main() {
    auto head = MyWidget{};
    return Application{head, Terminal{MouseMode::Move}}.run();
}
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
explicit Application(Widget& head, Terminal term = {});
```

Construct an Application object with the given head Widget and Terminal. The head Widget
is an l-value reference and must outlive the Application object.

```cpp
Application(Application const&) = delete;
Application(Application&&) = delete;
auto operator=(Application const&) -> Application& = delete;
auto operator=(Application&&) -> Application& = delete;
```

Application is non-copyable and non-movable.

---

### `Application::run`

```cpp
auto Application::run() -> int;
```

This launches the event loop:

- Read user input, creating Events.
- Feed each Event to the head Widget.
- Write updates to the Terminal.

Will continue to loop until `Application::quit` has been called. The return value is the integer code passed to `Application::quit`.

---

### `Application::quit`

```cpp
static void Application::quit(int code);
```

Set a flag to break out of the `Application::run` call on the next iteration. This can
be called from any event processor function, including any Widget virtual function event
handler (excluding `Widget::paint(...)`). The `code` parameter will be returned from the `Application::run` call.

If Terminal OS Signals are enabled (the default), `ctrl + c` will quit the Application
loop as well.

---

</details>

## 🧩 ox::Timer

[`#include <ox/timer.hpp`](../include/ox/timer.hpp)

`class Timer;`

Posts `event::Timer` objects at regular intervals to the EventQueue.

```cpp
struct MyWidget : Widget {
    Timer timer {*this, std::chrono::milliseconds{500}, true};
    int count{0};

    void timer() override { ++count; }
};
```

<details>
<summary><strong>Details</strong></summary>

This uses its own internal thread to enqueue Events. Each Timer is tied to a single
Widget, which will recieve the Events. Timer events are handled with the
`Widget::timer()` virtual function, typically you would update the Widget state, the
`Widget::paint(Canvas)` event handler will be called automatically after the timer event
handler.

The Widget lifetime is handled by a `LifetimeView` and will remain valid after moving a
Widget. Though not recommended for clarity, it is safe to delete a Widget without
deleting its associated Timer.


### 🏗️ Constructors

```cpp
explicit Timer(Widget& w, std::chrono::milliseconds duration, bool launch = false);
```

Construct a Timer to post to the EventQueue. The given Widget `w` will receive the Timer
events, sent every `duration`. If launch is `true`, the Timer will begin at
construction. The timer does not send an event until after the first `duration`
interval.

```cpp
Timer(Timer const&) = delete;
Timer(Timer&& other);
auto operator=(Timer const&) -> Timer& = delete;
auto operator=(Timer&& other) -> Timer&;
```

Move only.

---

### 🧹 Destructor

If the Timer is running, requests a stop and joins the thread. Check out
`zzz::TimerThread` for details, the gist is it owns a `std::jthread` which will join the thread on destruction. As of this writing, `zzz::TimerThread` checks for a stop request at least every 16 milliseconds.

---

### `Timer::start`

```cpp
void Timer::start();
```

Begin waiting on duration and posting events.

---

### `Timer::stop`

```cpp
void Timer::stop();
```

Requests a stop, returns immediately, does not wait on thread to fully quit.

---

### `Timer::id`

```cpp
[[nodiscard]]
auto Timer::id() const -> int;
```

Returns the unique Timer id for the instance.

---

### `Timer::duration`

```cpp
[[nodiscard]]
auto Timer::duration() const -> std::chrono::milliseconds;
```

Returns the duration the Timer was constructed with.

---

### `Timer::is_running`

```cpp
[[nodiscard]]
auto Timer::is_running() const -> bool;
```

Returns true if `start()` has been called and the Timer is currently running (i.e.,
`stop()` has not been called since the last `start()`).

---

</details>

## 🧩 ox::Focus

[`#include <ox/focus.hpp>`](../include/ox/focus.hpp)

`class Focus;`

Manages the focus of Widgets. When a Widget is in focus, it will receive keyboard input. Only one Widget can be in focus at a time.

```cpp
Focus::set(my_widget);

auto in_focus = Focus::get();
if (in_focus.valid()) { in_focus.get().foo(); }
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
Focus() = delete;
Focus(Timer const&) = delete;
Focus(Timer&& other) = delete;
auto operator=(Focus const&) -> Focus& = delete;
auto operator=(Focus&& other) -> Focus& = delete;
```

Everything is static, no need for instantiations.

---

### `Focus::set`

```cpp
static
void Focus::set(Widget& w);
```

Sets the current focus to `w`. Invokes the `Widget::focus_out` event handler on the
currently in focus Widget, if one exists, and `Widget::focus_in` on the passed in `w`.

---

### `Focus::get`

```cpp
static
auto Focus::get() -> LifetimeView<Widget>;
```

Returns a `LifetimeView` of the in focus Widget, which will not be valid if nothing is
in focus.

---

### `Focus::clear`

```cpp
static
void clear();
```

Clears the focus, removing any in focus Widget, invoking `Widget::focus_out` on it.

---

</details>

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
    put(c, {.x = 6, .y = 10}, 'X' | fg(XColor::Blue));
    put(c, {.x = 7, .y = 10}, "Hello!");

    put(c, {.x = 0, .y = 0}, shape::HLine{}, this->size.width);
    put(c, {.x = 0, .y = 0}, shape::VLine{}, 3, Brush{.background = RGB{0x337283}});
    put(c, {.x = 0, .y = 1}, shape::Box::dashed(), {.width = 10, .height = 4});

    put(c, shape::Box::bold());  // Outlines the Canvas.
}
```

<details>
<summary><strong>Details</strong></summary>

A Canvas is a light-weight 2D grid of Glyph objects. It provides a direct mapping to a
sub-region of the `ScreenBuffer`.

`put(...)` functions do bounds checking, and anything painted out of bounds of the
Canvas will be ignored. These functions are provided as convenience, as an alternative
with no bounds checking, individual Glyphs can be painted to the screen with
`Canvas::operator[]`. A Widget's Canvas starts at the top left with `Point{.x = 0, .y =
0}` and ends at the bottom right with `Point{.x = this->size.width - 1, .y =
this->size.height - 1}`.

Overloads without a `Color` or `Brush` will modify the `Glyph::symbol` members in the
Canvas, keeping the existing Brush. Overloads with `Color` will overwrite both the
symbol and the `Glyph::brush::foreground` members, and overloads with `Brush` will
overwrite the symbol and the `Glyph::brush` members.

```cpp
void put(Canvas c, Point at, Glyph const& item);
void put(Canvas c, Point at, Character auto item);
void put(Canvas c, Point at, GlyphString auto const& item);
void put(Canvas c, Point at, std::string_view element);
void put(Canvas c, Point at, std::u32string_view element);

void put(Canvas c, Point at, shape::HLine item, int length);
void put(Canvas c, Point at, shape::HLine item, int length, Color foreground);
void put(Canvas c, Point at, shape::HLine item, int length, Brush const& brush);

void put(Canvas c, Point at, shape::VLine item, int length);
void put(Canvas c, Point at, shape::VLine item, int length, Color foreground);
void put(Canvas c, Point at, shape::VLine item, int length, Brush const& brush);

void put(Canvas c, Point at, shape::Box const& item, Area size);
void put(Canvas c, Point at, shape::Box const& item, Area size, Color foreground);
void put(Canvas c, Point at, shape::Box const& item, Area size, Brush const& brush);

void put(Canvas c, shape::Box const& item);
void put(Canvas c, shape::Box const& item, Color foreground);
void put(Canvas c, shape::Box const& item, Brush const& brush);
```

---

</details>

## 🔧 ox::fill(...)

[`#include <ox/put.hpp>`](../include/ox/put.hpp)

A function overload set to fill a 2D space on a Canvas. It fills the entire Canvas object given to it.

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
    fill(sub_region, 'X' | bg(XColor::Red) | fg(XColor::Black));
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
void fill(Canvas c, Brush b);
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

This function should not be needed in most circumstances, the Canvas is provided cleared on entry to the `Widget::paint(Canvas)` event handler, unless a subclass or parent Widget has written over it.

```cpp
void clear(Canvas c);
```

---

</details>

## 🧩 ox::Widget

[`#include <ox/widget.hpp>`](../include/ox/widget.hpp)

The base class unit of graphical display and user interaction. Each Widget type is a
combination of handling user input and painting its state to the screen on its allocated
screen space (Canvas).

Event handlers are virtual functions that are overridden to provide custom state changes; the paint event handler allows output of that state onto the screen.

```cpp
code
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
Widget(FocusPolicy fp, SizePolicy sp);
```

```cpp
Widget(Widget const&) = delete;
Widget(Widget&& other);

auto operator=(Widget const&) -> Widget& = delete;
auto operator=(Widget&& other) -> Widget&;
```

---

### 🧹 Destructor

```cpp
virtual ~Widget() = default;
```

---

### Public Objects

```cpp
FocusPolicy focus_policy;
SizePolicy size_policy;
Terminal::Cursor cursor = std::nullopt;
bool active = true;

Point at = {.x = 0, .y = 0};
Area size = {.width = 0, .height = 0};
std::shared_ptr<Widget*> lifetime = std::make_shared<Widget*>(this);
```

---

### `Widget::mouse_press`

```cpp
virtual void mouse_press(Mouse);
```

---

### `Widget::mouse_release`

```cpp
virtual void mouse_release(Mouse);
```

---

### `Widget::mouse_wheel`

```cpp
virtual void mouse_wheel(Mouse);
```

---

### `Widget::mouse_move`

```cpp
virtual void mouse_move(Mouse);
```

// enable in application constructor

---

### `Widget::mouse_enter`

```cpp
virtual void mouse_enter();
```

// enable in application constructor with mouse move

---

### `Widget::mouse_leave`

```cpp
virtual void mouse_leave();
```
// enable in application constructor with mouse move

---

### `Widget::focus_in`

```cpp
virtual void focus_in();
```

// only even recieved if foucs policy is not none

---

### `Widget::focus_out`

```cpp
virtual void focus_out();
```

---

### `Widget::key_press`

```cpp
virtual void key_press(Key);
```

---

### `Widget::key_release`

```cpp
virtual void key_release(Key);
```
// enable in application constructor, has issues, terminal not built for this, linux only?

---

### `Widget::resize`

```cpp
virtual void resize(Area old_size);
```

---

### `Widget::timer`

```cpp
virtual void timer();
```

// Need a Timer object pointing to *this

---

### `Widget::paint`

```cpp
virtual void paint(Canvas);
```

// never need to call this yourself, done automatically by Application class.

---

### `Widget::get_children`

```cpp
virtual auto get_children() -> zzz::Generator<Widget&>;
virtual auto get_children() const -> zzz::Generator<Widget const&>;
```

// Only need to override if you are creating a new 'layout' type that owned child Widgets.

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

The returned `sl::Identifier` can be used to disconnect the Slot from the Signal. Any number of Slots can be connected to the same Signal.

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

The pipe (`|`) operator is overloaded for Widget and FocusPolicy to set the FocusPolicy inline with Widget construction.

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

[`#include <ox/widget.hpp>`](../include/ox/widget.hpp)

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

The parent layout type provides context to each Widget's SizePolicy, if it is a Row,
then the policies dictate the width dimension, if it is a Column, the height dimension,
if it is a Suspended layout, then `SizePolicy::suspended` is the only correct policy.

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
    static constexpr auto suspended(Area size) -> SizePolicy;
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
corresponding `track(...)` function to construct a LifetimeView. LifetimeView is useful for Signals/Slots to check if any captured Widgets are no longer around.

```cpp
auto my_widg = Widget{...};
auto lv = track(my_widg);
auto widg2 = std::move(my_widg); // lv will now point here.
if (lv.valid()) { my_widg.get().do_thing(); }
```

<details>
<summary><strong>Details</strong></summary>

It does this by accessing the `lifetime` shared pointer within the Widget class and
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

Returns a reference to the tracked Widget, cast to the ConcreteType provided at construction. Undefined behaviour if the tracked Widget is not valid.

---

</details>

## 🔢 ox::Align

[`#include <ox/align.hpp>`](../include/ox/align.hpp)

```cpp
enum class Align;
enum class VAlign;
```

Describes horizontal (`Align`) or vertical (`VAlign`) alignment of text. Used by various Widgets.

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

## 🧩 ox::Bordered

[`#include <ox/bordered.hpp>`](../include/ox/bordered.hpp)

```cpp
template <WidgetDerived ChildWidget>
class Bordered;
```

description

```cpp
code
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
```

---

### `Bordered::...`

```cpp
```

details

---

</details>

## 🧩 ox::Button

[`#include <ox/button.hpp>`](../include/ox/button.hpp)

`class Button;`

description

```cpp
code
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
```

---

### `Button::...`

```cpp
```

details

---

</details>

## 🧩 ox::CheckBox

[`#include <ox/checkbox.hpp>`](../include/ox/checkbox.hpp)

`class CheckBox;`

description

```cpp
code
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
```

---

### `CheckBox::...`

```cpp
```

details

---

</details>

## 🧩 ox::DataTable

[`#include <ox/datatable.hpp>`](../include/ox/datatable.hpp)

`class DataTable;`

description

```cpp
code
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
```

---

### `DataTable::...`

```cpp
```

details

---

</details>

## 🧩 ox::Label

[`#include <ox/label.hpp>`](../include/ox/label.hpp)

`class Label;`

description

```cpp
code
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
```

---

### `Label::...`

```cpp
```

details

---

</details>

## 🧩 ox::Column

[`#include <ox/layout.hpp>`](../include/ox/layout.hpp)

`class Colum;`

description

```cpp
code
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
```

---

### `Column::...`

```cpp
```

details

---

</details>

## 🧩 ox::Row

[`#include <ox/layout.hpp>`](../include/ox/layout.hpp)

`class Row;`

description

```cpp
code
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
```

---

### `Row::...`

```cpp
```

details

---

</details>

## 🧩 ox::LineEdit

[`#include <ox/lineedit.hpp>`](../include/ox/lineedit.hpp)

`class LineEdit;`

description

```cpp
code
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
```

---

### `LineEdit::...`

```cpp
```

details

---

</details>

## 🧩 ox::PixelGrid

[`#include <ox/pixelgrid.hpp>`](../include/ox/pixelgrid.hpp)

`class PixelGrid;`

description

```cpp
code
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
```

---

### `PixelGrid::...`

```cpp
```

details

---

</details>

## 🧩 ox::ScrollBar

[`#include <ox/scrollbar.hpp>`](../include/ox/scrollbar.hpp)

`class ScrollBar;`

description

```cpp
code
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
```

---

### `ScrollBar::...`

```cpp
```

details

---

</details>

## 🧩 ox::TextBox

[`#include <ox/textbox.hpp>`](../include/ox/textbox.hpp)

`class TextBox;`

description

```cpp
code
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
```

---

### `TextBox::...`

```cpp
```

details

---

</details>

## 🧩 ox::Glyph

[`#include <ox/core/glyph.hpp>`](../include/ox/core/glyph.hpp)

`...`

description

```cpp
```

<details>
<summary><strong>Details</strong></summary>

---

</details>

## 🧩 ox::Color

[`#include <ox/core/glyph.hpp>`](../include/ox/core/glyph.hpp)

`...`

description
hsl, rgb, bg, fg, XColor, TrueColor

```cpp
```

<details>
<summary><strong>Details</strong></summary>

---

</details>

## 🧩 ox::Point

[`#include <ox/core/glyph.hpp>`](../include/ox/core/glyph.hpp)

`...`

description

```cpp
```

<details>
<summary><strong>Details</strong></summary>

---

</details>

## 🧩 ox::Brush

[`#include <ox/core/glyph.hpp>`](../include/ox/core/glyph.hpp)

`...`

description

```cpp
```

<details>
<summary><strong>Details</strong></summary>

---

</details>

## ox::Trait

[`#include <ox/core/glyph.hpp>`](../include/ox/core/glyph.hpp)

`...`

description

```cpp
```

<details>
<summary><strong>Details</strong></summary>

---

</details>


## ox::Traits

[`#include <ox/core/glyph.hpp>`](../include/ox/core/glyph.hpp)

`...`

description

```cpp
```

<details>
<summary><strong>Details</strong></summary>

---

</details>

## ox::GlyphString

[`#include <ox/core/glyph.hpp>`](../include/ox/core/glyph.hpp)

`...`

description

```cpp
```

<details>
<summary><strong>Details</strong></summary>

---

</details>

## 🧩 ox::Terminal

[`#include <ox/core/terminal.hpp>`](../include/ox/core/terminal.hpp)

`class Terminal;`

description

```cpp
code
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
```

---

### `Terminal::...`

```cpp
```

details

---

</details>

## 🧩 ox::Canvas

[`#include <ox/core/terminal.hpp>`](../include/ox/core/terminal.hpp)

`struct Canvas;`

description

```cpp
code
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
```

---

### `Canvas::...`

```cpp
```

details

---

</details>

## 🧩 ox::ScreenBuffer

[`#include <ox/core/terminal.hpp>`](../include/ox/core/terminal.hpp)

`class ScreenBuffer;`

Holds the 2D matrix of Glyphs that make up the screen display state. This is used by the
core of the library and direct access should not be needed by the typical user of this
library.

## 🧩 ox::EventQueue

[`#include <ox/core/events.hpp>`](../include/ox/core/events.hpp)

`using EventQueue = ConcurrentQueue<Event>;`

A multi-producer single-consumer queue of Events for the Application to process. This is
used by the core of the library and direct access should not be needed by the typical
user of this library.

## 🔢 ox::Event

A `std::variant` of input event types. This is used by the core of the library and
direct access should not be needed by the typical user of this library.

```cpp
using Event = std::variant<esc::MousePress,
                           esc::MouseRelease,
                           esc::MouseWheel,
                           esc::MouseMove,
                           esc::KeyPress,
                           esc::KeyRelease,
                           esc::Resize,
                           event::Timer,
                           event::Custom,
                           event::Interrupt>;
```
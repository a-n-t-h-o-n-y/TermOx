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
auto run() -> int;
```

This launches the event loop:

- Read user input, creating Events.
- Feed each Event to the head Widget.
- Write updates to the Terminal.

Will continue to loop until `Application::quit` has been called. The return value is the
integer code passed to `Application::quit`.

---

### `Application::quit`

```cpp
static void quit(int code);
```

Set a flag to break out of the `Application::run` call on the next iteration. This can
be called from any event processor function, including any Widget virtual function event
handler (excluding `Widget::paint(...)`). The `code` parameter will be returned from the
`Application::run` call.

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
`zzz::TimerThread` for details, the gist is it owns a `std::jthread` which will join the
thread on destruction. As of this writing, `zzz::TimerThread` checks for a stop request
at least every 16 milliseconds.

---

### `Timer::start`

```cpp
void start();
```

Begin waiting on duration and posting events.

---

### `Timer::stop`

```cpp
void stop();
```

Requests a stop, returns immediately, does not wait on thread to fully quit.

---

### `Timer::id`

```cpp
auto id() const -> int;
```

Returns the unique Timer id for the instance.

---

### `Timer::duration`

```cpp
auto duration() const -> std::chrono::milliseconds;
```

Returns the duration the Timer was constructed with.

---

### `Timer::is_running`

```cpp
auto is_running() const -> bool;
```

Returns true if `start()` has been called and the Timer is currently running (i.e.,
`stop()` has not been called since the last `start()`).

---

</details>

## 🧩 ox::Focus

[`#include <ox/focus.hpp>`](../include/ox/focus.hpp)

`class Focus;`

Manages the focus of Widgets. When a Widget is in focus, it will receive keyboard input.
Only one Widget can be in focus at a time.

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
static void set(Widget& w);
```

Sets the current focus to `w`. Invokes the `Widget::focus_out` event handler on the
currently in focus Widget, if one exists, and `Widget::focus_in` on the passed in `w`.

---

### `Focus::get`

```cpp
static auto get() -> LifetimeView<Widget>;
```

Returns a `LifetimeView` of the in focus Widget, which will not be valid if nothing is
in focus.

---

### `Focus::clear`

```cpp
static void clear();
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
            put(c, at, U'•' | fg(XColor::Red));
        }
    }

    void mouse_press(Mouse m) override
    {
        if (m.button == Mouse::Button::Left)
            points_.emplace(m.at);
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
Terminal::Cursor cursor = std::nullopt;
bool active = true;

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
virtual void mouse_press(Mouse);
```

Event handler for receiving mouse press events that occur within the bounds of this
Widget. If the Widget is a layout, it will not recieve mouse press events intended for
its children.

Events from `Button::ScrollUp` and `Button::ScrollDown` are not received here, see
`Widget::mouse_wheel`.

---

### `Widget::mouse_release`

```cpp
virtual void mouse_release(Mouse);
```

Event handler for receiving mouse release events that occur within the bounds of this
Widget. If the Widget is a layout, it will not recieve mouse press events intended for
its children.

---

### `Widget::mouse_wheel`

```cpp
virtual void mouse_wheel(Mouse);
```

Event handler for receiving events generated by either `Button::ScrollUp` or
`Button::ScrollDown`. Each 'click' or increment of the mouse wheel generates a single
event.

---

### `Widget::mouse_move`

```cpp
virtual void mouse_move(Mouse);
```

Event handler for events generated by movement of the mouse over the Widget. This has to
be enabled in the `Terminal` constructor with the correct `MouseMode` option, either
`MouseMode::Move` or `MouseMode::Drag`.

---

### `Widget::mouse_enter`

```cpp
virtual void mouse_enter();
```

Event handler for events generated by movement of the mouse into the Widget from another
Widget. This has to be enabled in the `Terminal` constructor with the correct
`MouseMode` option, either `MouseMode::Move` or `MouseMode::Drag`.

---

### `Widget::mouse_leave`

```cpp
virtual void mouse_leave();
```

Event handler for events generated by movement of the mouse out of the Widget and into
another Widget. This has to be enabled in the `Terminal` constructor with the correct
`MouseMode` option, either `MouseMode::Move` or `MouseMode::Drag`.

---

### `Widget::focus_in`

```cpp
virtual void focus_in();
```

Called when this Widget recieves keyboard focus. The Widget's `focus_policy` must not be
`FocusPolicy::None` to receive focus.

---

### `Widget::focus_out`

```cpp
virtual void focus_out();
```

Called when this Widget gives up keyboard focus. The Widget's `focus_policy` must not be
`FocusPolicy::None` to receive focus.

---

### `Widget::key_press`

```cpp
virtual void key_press(Key);
```

Called on keyboard key presses when this Widget has focus. See `KeyMode` for info on
what types of keypresses are generated.

---

### `Widget::key_release`

```cpp
virtual void key_release(Key);
```

Called on keyboard key release when this Widget has focus. `KeyMode::Raw` must be set in
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
virtual void timer();
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

## 🧩 ox::Mouse

[`#include <ox/core/events.hpp>`](../include/ox/core/events.hpp)

`struct Mouse;`

Contains data about a Mouse user input event.

<details>
<summary><strong>Details</strong></summary>

```cpp
struct Mouse {
    enum class Button : std::uint16_t {
        Left,
        Middle,
        Right,
        None,
        ScrollUp,
        ScrollDown,
        Six,
        Seven,
        Eight,
        Nine,
        Ten,
        Eleven,
    };

    Point at;
    Button button;

    struct Modifiers {
        bool shift = false;
        bool ctrl = false;
        bool alt = false;
    } modifiers;
};
```

---

</details>

## 🧩 ox::Key

[`#include <ox/core/events.hpp>`](../include/ox/core/events.hpp)

`enum class Key;`

Contains data about a Key user input event.

<details>
<summary><strong>Details</strong></summary>

```cpp
/// see 'esc/key.hpp' in Escape lib dependency for full listing.
enum class Key : char32_t {
    // ...
    Nine,
    Colon,
    Semicolon,
    LessThan,
    Equals,
    GreaterThan,
    QuestionMark,
    AtSign,
    A,
    B,
    C,
    // ...
};

/**
 * The Mod enum is used to represent modifier keys on a keyboard.
 * @details It is designed to be used with the Key enum to represent key combinations.
 * These are only generated when KeyMode::Raw is set. In KeyMode::Normal these not seen
 * as separate keys, for instance ctrl + a is seen as Key::StartOfHeading and shift + a
 * is seen as Key::A.
 */
enum class Mod : std::underlying_type_t<Key> {
    // Last bit of unicode currently used is (1 << 20).
    Shift = (std::underlying_type_t<Key>{1} << 30),
    Ctrl = (std::underlying_type_t<Key>{1} << 29),
    Alt = (std::underlying_type_t<Key>{1} << 28),
    Meta = (std::underlying_type_t<Key>{1} << 27)
};

/**
 * Return true if \p mod is set on \p key.
 */
constexpr auto is_set(Key key, Mod mod) -> bool;
```

---

</details>

## 🧩 ox::Border

[`#include <ox/bordered.hpp>`](../include/ox/bordered.hpp)

`struct Border;`

Visual description of a Border surrounding a Widget. To be used with `Bordered` class.

```cpp
struct Border {
    shape::Box box = shape::Box::square();
    Brush brush = {};  // Only applies to box.
    Label label = {};
};
```

<details>
<summary><strong>Details</strong></summary>

The `Border::brush` member only applies to the drawing of the Box. The Label has its own
internal Brush it will use to paint its text. Alignment of the Label will properly align
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
        .brush = {.foreground = XColor::BrightBlack},
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
    Brush brush = {};
    Label label = {};
};

Divider(Options x);
```

The `Divider::brush` member only applies to the Line, and not the Label, The Label has
its own internal Brush.

---

### Public Objects

```cpp
Lines lines;
Brush brush;
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
    .press_mod = [](Label& l) { l.brush.foreground = XColor::BrightWhite; },
    .focus_mod = [](Label& l) { l.brush.foreground = XColor::BrightCyan; },
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
auto shape_gradient(auto const& shape, TrueColor one, TrueColor two)
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
    Brush in_focus_brush;
};

struct Options {
    State state = State::UnChecked;
    Display display = {
        .checked = U"[X]"_gs,
        .unchecked = U"[ ]"_gs,
        .in_focus_brush = {.traits = Trait::Bold},
    };
    FocusPolicy focus_policy = FocusPolicy::Strong;
};

CheckBox(Options x = {});
```

Construct a CheckBox Widget. `state` is the initial state of the CheckBox.

The display object describes the look of each state in Glyphs and the Brush to use when
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
dt.add_column("one", Align::Center, XColor::Blue);
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
    Color background = TermColor::Default;
    Color foreground_cell = TermColor::Default;
    Color foreground_line = TermColor::Default;
    SizePolicy size_policy = SizePolicy::flex();
};

DataTable(Options x = {});
```

---

### `DataTable::add_column`

```cpp
void add_column(std::string heading,
                Align align = Align::Left,
                Color foreground = TermColor::Default);
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
</details>

## 🧩 ox::Label

[`#include <ox/label.hpp>`](../include/ox/label.hpp)

`class Label : public Widget;`

A Widget displaying a static single line of text. The text is not editable, but it can be set directly via its public data members. The text is centered vertically, its horizontal alignment is determined by the `Align` constructor parameter.

```cpp
auto label = Label{{
    .text = "A Label",
    .align = Align::Left,
    .brush = {.foreground = XColor::BrightWhite},
}};
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
struct Options {
    std::string text = "";
    Align align = Align::Center;
    Brush brush = {};
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
horizontal alignment, a `Brush` to apply to each cell of the Label, and a `SizePolicy`.

---

### Public Objects

```cpp
std::string text;
Align align;
Brush brush;
```

The text to be displayed, the alignment of the text, and the Brush to use when painting.

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
    .brush = {.foreground = XColor::BrightWhite},
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
    Brush brush = {};
    std::locale locale = std::locale{};
    std::string format = "{}";
    SizePolicy size_policy = SizePolicy::flex();
};

BasicNumberLabel(Options x = {});
```

Construct a BasicNumberLabel Widget. The `value` is the number to be displayed, `align`
is the horizontal alignment of the text, the `brush` is the Brush to use when painting,
the `locale` is the locale to use for formatting, and the `format` is the format string
to use for formatting.

---

### Public Objects

```cpp
NumberType value;
Align align;
Brush brush;
std::locale locale;
std::string format;
```

The number to be displayed, the alignment of the text, the Brush to use when painting,
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

A layout holding a single Widget that 'floats' in the center of the layout. The child
Widget must have a `SizePolicy::suspended(...)` policy, which sets the Widget to a fixed size.

```cpp
auto sus = Suspended{
    TextBox{{
        .brush = {.background = XColor::BrightBlack},
        .focus_policy = FocusPolicy::Strong,
    }} | SizePolicy::suspended({.width = 40, .height = 5}),
};
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
explicit Suspended(ChildType&& child, Glyph fill = {U' '});
```

The constructor takes a single Widget and a fill character. The fill character is used
to fill the space around the child Widget. The child Widget must have a
`SizePolicy::suspended(...)` policy, which sets the Widget to a fixed size.

### Public Objects

```cpp
ChildType child;
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
    .text_brush = {.foreground = XColor::BrightWhite},
    .background = XColor::BrightBlue,
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
    Brush text_brush = {};
    Align align = Align::Left;
    Color background = TermColor::Default;
    std::string ghost_text = "";
    Brush ghost_text_brush = {.foreground = XColor::BrightBlack};
    std::optional<std::function<bool(char)>> validator = std::nullopt;
    FocusPolicy focus_policy = FocusPolicy::Strong;
    SizePolicy size_policy = SizePolicy::flex();
};

LineEdit(Options x = {});
```

Construct a LineEdit Widget. The `text` is the initial text to be displayed, the
`text_brush` is the Brush to use when painting the text, `align` is the horizontal
alignment of the text, `background` is the background color of the LineEdit,
`ghost_text` is the text to be displayed when the LineEdit is empty, `ghost_text_brush`
is the Brush to use when painting the ghost text, `validator` is a function that will be
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
std::string text;
Brush text_brush;
Align align;
Color background;
std::string ghost_text;  // Visible if text is empty.
Brush ghost_text_brush;
std::optional<std::function<bool(char)>> validator;
```

---

</details>

## 🧩 ox::PixelGrid

[`#include <ox/pixelgrid.hpp>`](../include/ox/pixelgrid.hpp)

`class PixelGrid : public Widget;`

A 2D grid of pixels, each pixel is a `TrueColor` object. Each cell on the terminal holds 2 'pixels', the `▄` character is used to do the drawing.

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
    std::vector<TrueColor> pixels;

    auto operator[](Point p) -> TrueColor&;
    auto operator[](Point p) const -> TrueColor;
};

PixelGrid(Area area);

PixelGrid(Matrix matrix);
```

The first constructor takes an `Area` object, which is the size of the grid.

The second constructor takes a `Matrix` object, which is a 2D array of `TrueColor`
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
    Brush brush = {};
    std::chrono::milliseconds scroll_settle_time = std::chrono::milliseconds{500};
};

ScrollBar(Options x = {});
```

Construct a ScrollBar Widget. The `scrollable_length` is the length of the scrollable
area, the `position` is the current position of the scroll bar, the `brush` is the Brush
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
Brush brush;
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
    .wrap = Wrap::Word,
    .align = Align::Center,
    .brush = {.foreground = XColor::BrightWhite, .traits = Trait::Bold},
}};

auto sb = ScrollBar{};

link(tb, sb);
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

```cpp
enum class Wrap { Any, Word };

struct Options {
    std::string text = {};  // UTF8 encoded
    Wrap wrap = Wrap::Word;
    Align align = Align::Left;
    Brush brush = {};
    FocusPolicy focus_policy = FocusPolicy::None;
    SizePolicy size_policy = SizePolicy::flex();
};

TextBox(Options x = {});
```

Construct a TextBox Widget. The `text` is the initial text to be displayed, the `wrap`
is the wrapping mode (`Any` will wrap at any character, and `Word` will wrap on word
boundaries), `align` is the horizontal alignment of the text, `brush` is the Brush to
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
std::string text;
Wrap wrap;
Align align;
Brush brush;
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

## ox::Trait

[`#include <ox/core/glyph.hpp>`](../include/ox/core/glyph.hpp)

`enum class Trait;`

Visual traits that can be applied to text on the terminal screen. Not all terminal
emulators support all Traits.

<details>
<summary><strong>Details</strong></summary>

```cpp
enum class Trait : std::uint16_t {
    None = 0,
    Standout = 1 << 0,
    Bold = 1 << 1,
    Dim = 1 << 2,
    Italic = 1 << 3,
    Underline = 1 << 4,
    Blink = 1 << 5,
    Inverse = 1 << 6,
    Invisible = 1 << 7,
    Crossed_out = 1 << 8,
    Double_underline = 1 << 9,
};
```

A RemoveTrait struct is provided to remove the given Trait, to be used with Glyph and
GlyphString pipe operators.

```cpp
struct RemoveTrait {
    Trait trait;
};

constexpr auto remove_trait(Trait t) -> RemoveTrait;
```

---

</details>

## ox::Traits

[`#include <ox/core/glyph.hpp>`](../include/ox/core/glyph.hpp)

`using Traits = detail::Mask<Trait>;`

Bitmask type for combining multiple traits into a single object.

```cpp
auto traits = Trait::Bold | Trait::Inverse;
```

## 🧩 ox::Color

[`#include <ox/core/glyph.hpp>`](../include/ox/core/glyph.hpp)

`using Color = std::variant<XColor, TrueColor, TermColor>;`

Describes a color that can appear on the terminal screen.

```cpp
Color c = XColor::Blue;
c = TrueColor{RGB{0x432654}};
c = TrueColor{HSL{
    .hue = 250,
    .saturation = 30,
    .lightness = 70,
}};
c = TermColor::Default;
```

<details>
<summary><strong>Details</strong></summary>

A Color can be an XTerm color palette index (`XColor`), a TrueColor (`HSL` or `RGB`), or
a Terminal's default color (`TermColor::Default`).

### XColor

```cpp
/**
 * Represents a color index for XTerm like palette.
 * @details The color index is a value from 0 to 255.
 *     [0, 7] ----- Classic XTerm system colors.
 *     [8, 15] ---- 'Bright' versions of the classic colors.
 *     [16, 230] -- 216 non-system colors.
 *     [231, 255] - Grayscale colors.
 * The actual color values are defined by your terminal.
 * @details This provides the first 16 colors of the XTerm palette by name.
 * @see https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit
 * @see https://www.ditig.com/publications/256-colors-cheat-sheet
 */
struct XColor {
    std::uint8_t value;

    static XColor const Black;
    static XColor const Red;
    static XColor const Green;
    static XColor const Yellow;
    static XColor const Blue;
    static XColor const Magenta;
    static XColor const Cyan;
    static XColor const White;
    static XColor const BrightBlack;
    static XColor const BrightRed;
    static XColor const BrightGreen;
    static XColor const BrightYellow;
    static XColor const BrightBlue;
    static XColor const BrightMagenta;
    static XColor const BrightCyan;
    static XColor const BrightWhite;

    constexpr bool operator==(XColor const&) const = default;
    constexpr bool operator!=(XColor const&) const = default;
};
```

The static members are provided for convenience, but any XColor can be constructed
directly by placing the color index in the `value` member.

---

### TrueColor

```cpp
struct TrueColor {
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;

    constexpr TrueColor(RGB x);
    constexpr TrueColor(HSL x);

    constexpr bool operator==(TrueColor const&) const = default;
    constexpr bool operator!=(TrueColor const&) const = default;
};

using TColor = TrueColor;
```

24 bit true color for the terminal display, if your terminal supports it. Constructible
from either an RGB or HSL value, with HSL there is a conversion to RGB internally. The
constructor is non-explicit so it can be directly created from an `RGB` or `HSL` struct
if the expected type is a `TrueColor` (not a `Color`).

---

### TermColor

```cpp
enum class TermColor : bool { Default };
```

The default color the terminal uses for either background or foreground, depending on
where it is assigned in a `Brush`. This is the default Color assigned in Brush for both
foreground and background. The actual color can be changed in the `Terminal` class by
assigning to the following members:

```cpp
Color Terminal::foreground = TermColor::Default;
Color Terminal::background = TermColor::Default;
```

If left as `TermColor::Default` it will use the Terminal's settings.

---

### Foreground / Background

A Color can be used for either a foreground or background color, depending on where it
is assigned in a `Brush`. When using pipe operators the library provides the following
functions to help assign colors to the correct spot in a Brush:

```cpp
constexpr auto foreground(Color c) -> ColorFG;
constexpr auto fg(Color c) -> ColorFG;

constexpr auto background(Color c) -> ColorBG;
constexpr auto bg(Color c) -> ColorBG;
```

---

</details>

## 🧩 ox::Brush

[`#include <ox/core/glyph.hpp>`](../include/ox/core/glyph.hpp)

`struct Brush;`

A collection of foreground color, background color, and traits. It is used to 'paint' a
symbol to the terminal screen.

```cpp
auto b = Brush{
    .foreground = RGB{0x32A573},
    .background = XColor::Blue,
    .traits = Trait::Bold | Trait::Underline,
};
```

<details>
<summary><strong>Details</strong></summary>

```cpp
struct Brush {
    Color background = TermColor::Default;
    Color foreground = TermColor::Default;
    Traits traits = Traits{Trait::None};

    constexpr bool operator==(Brush const&) const = default;
    constexpr bool operator!=(Brush const&) const = default;
};
```

---

</details>

## 🧩 ox::Glyph

[`#include <ox/core/glyph.hpp>`](../include/ox/core/glyph.hpp)

`struct Glyph;`

The unit of visual display for a single cell on the terminal. It contains a symbol,
colors, and traits.

```cpp
auto g = Glyph{.symbol = 'o', .brush = {.foreground = XColor::Blue}};
```

<details>
<summary><strong>Details</strong></summary>

```cpp
struct Glyph {
    char32_t symbol = U' ';
    Brush brush = {};

    constexpr bool operator==(Glyph const&) const = default;
    constexpr bool operator!=(Glyph const&) const = default;
};
```

---

</details>

## 💡 ox::GlyphString

[`#include <ox/core/glyph.hpp>`](../include/ox/core/glyph.hpp)

```cpp
template <typename T>
concept GlyphString =
    std::ranges::forward_range<T> && std::same_as<std::ranges::range_value_t<T>, Glyph>;

template <typename T>
concept InsertableGlyphString = GlyphString<T> && requires(T& t, Glyph g) {
    { *std::inserter(t, std::begin(t)) = g } -> std::same_as<std::insert_iterator<T>&>;
};
```

A C++20 concept defining what it means to be a string of Glyphs. Any library function
that needs to return a GlyphString like type will return a `std::vector<Glyph>`.

<details>
<summary><strong>Details</strong></summary>

### User Defined Literals

```cpp
auto operator""_gs(char32_t const* str, std::size_t count) -> std::vector<Glyph>;
auto operator""_gs(char const* str, std::size_t count) -> std::vector<Glyph>;
```

GlyphString User Defined Literals to generate a `std::vector<Glyph>`.

```cpp
auto glyph_string = "Hello, World"_gs;
auto glyph_string_32 = U"Wow!"_gs;
```

---

</details>

## 🔧 Glyph And GlyphString Operator Overloads

The library provides a number of convinience operators for modification/creation of
Glyphs and GlyphStrings.

<details>
<summary><strong>Details</strong></summary>

### Additional Concepts

These are used in the following operator overloads.

```cpp
template <typename T>
concept Character = std::same_as<T, char> || std::same_as<T, signed char> ||
                    std::same_as<T, unsigned char> || std::same_as<T, char8_t> ||
                    std::same_as<T, char16_t> || std::same_as<T, char32_t>;

template <typename T>
concept Attribute =
    std::same_as<T, ColorBG> || std::same_as<T, ColorFG> || std::same_as<T, Trait> ||
    std::same_as<T, RemoveTrait> || std::same_as<T, Brush>;
```

---

### operator| Pipe Operator

```cpp
// ColorBG
constexpr auto operator|(Character auto symbol, ColorBG c) -> Glyph;
constexpr auto operator|(Glyph g, ColorBG c) -> Glyph;
constexpr auto operator|(GlyphString auto&& gs, ColorBG c) -> decltype(auto);

// ColorFG
constexpr auto operator|(Character auto symbol, ColorFG c) -> Glyph;
constexpr auto operator|(Glyph g, ColorFG c) -> Glyph;
constexpr auto operator|(GlyphString auto&& gs, ColorFG c) -> decltype(auto);

// Trait
constexpr auto operator|(Character auto symbol, Trait t) -> Glyph;
constexpr auto operator|(Glyph g, Trait t) -> Glyph;
constexpr auto operator|(GlyphString auto&& gs, Trait t) -> decltype(auto);

// RemoveTrait
constexpr auto operator|(Glyph g, RemoveTrait t) -> Glyph;
constexpr auto operator|(GlyphString auto&& gs, RemoveTrait t) -> decltype(auto);

// Brush
constexpr auto operator|(Character auto symbol, Brush b) -> Glyph;
constexpr auto operator|(Glyph g, Brush b) -> Glyph;
constexpr auto operator|(GlyphString auto&& gs, Brush b) -> decltype(auto);

// Attribute
auto operator|(std::u32string_view sv, Attribute auto attr) -> std::vector<Glyph>;
auto operator|(std::string_view sv, Attribute auto attr) -> std::vector<Glyph>;
```

---

### operator+ Plus Operator

```cpp
auto operator+(Glyph const& lhs, Glyph const& rhs) -> std::vector<Glyph>;
auto operator+(Glyph const& lhs, Character auto rhs) -> std::vector<Glyph>;
auto operator+(Character auto lhs, Glyph const& rhs) -> std::vector<Glyph>;

template <InsertableGlyphString T>
auto operator+(T lhs, std::string_view rhs) -> T;

template <InsertableGlyphString T>
auto operator+(std::string_view lhs, T rhs) -> T;

template <InsertableGlyphString T>
auto operator+(T lhs, std::u32string_view rhs) -> T;

template <InsertableGlyphString T>
auto operator+(std::u32string_view lhs, T rhs) -> T;

template <InsertableGlyphString T>
auto operator+(T lhs, GlyphString auto const& rhs) -> T;

template <InsertableGlyphString T>
auto operator+(T lhs, Glyph const& rhs) -> T;

template <InsertableGlyphString T>
auto operator+(T lhs, Character auto rhs) -> T;

template <InsertableGlyphString T>
auto operator+(Glyph const& lhs, T rhs) -> T;

template <InsertableGlyphString T>
auto operator+(Character auto lhs, T rhs) -> T;

auto operator+(Glyph const& lhs, std::string_view rhs) -> std::vector<Glyph>;
auto operator+(Glyph const& lhs, std::u32string_view rhs) -> std::vector<Glyph>;
auto operator+(std::string_view lhs, Glyph const& rhs) -> std::vector<Glyph>;
auto operator+(std::u32string_view lhs, Glyph const& rhs) -> std::vector<Glyph>;
```

---

### operator+= Plus Assignment Operator

```cpp
template <InsertableGlyphString T>
auto operator+=(T& lhs, GlyphString auto const& rhs) -> T&;

template <InsertableGlyphString T>
auto operator+=(T& lhs, std::u32string_view rhs) -> T&;

template <InsertableGlyphString T>
auto operator+=(T& lhs, std::string_view rhs) -> T&;

template <InsertableGlyphString T>
auto operator+=(T& lhs, Glyph const& rhs) -> T&;

template <InsertableGlyphString T>
auto operator+=(T& lhs, Character auto rhs) -> T&;
```

---

</details>

## 🧩 ox::Terminal

[`#include <ox/core/terminal.hpp>`](../include/ox/core/terminal.hpp)

`class Terminal;`

Represents the terminal itself, providing an event loop and screen writing tools.
Application object is constructed with a Terminal object.

```cpp
auto t = Terminal{{
    .mouse_mode = MouseMode::Move,
    .key_mode = KeyMode::Raw,
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
    KeyMode key_mode = KeyMode::Normal;
    Signals signals = Signals::On;
    Color foreground = TermColor::Default;
    Color background = TermColor::Default;
};

Terminal(Options options);

Terminal(MouseMode mouse_mode = MouseMode::Basic,
         KeyMode key_mode = KeyMode::Normal,
         Signals signals = Signals::On,
         Color foreground_ = TermColor::Default,
         Color background_ = TermColor::Default);
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

`KeyMode` determines the way key events are generated:

```cpp
/**
 * `Normal` will generate KeyPress Events and auto-repeat if key is held down;
 * `Raw` will generate KeyPress and KeyRelease Events, the shift key is not applied with
 * other keys, each key press and release is its own event, all letters are lowercase.
 * @details `Raw` mode is experimental, typically requiring superuser privileges, should
 * be avoided.
 */
enum class KeyMode : bool { Normal, Raw };
```

`Signals` determines if ctrl-key signals will be generated or will just generate normal
key press events:

```cpp
/**
 * `On` will generate signals from ctrl-[key] presses; `Off` will not.
 */
enum class Signals : bool { On, Off };
```

`foreground_` and `background_` determine the default colors for the Terminal.

```cpp
Terminal(Terminal const&) = delete;
Terminal(Terminal&&) = default;
auto operator=(Terminal const&) -> Terminal& = delete;
auto operator=(Terminal&&) -> Terminal& = default;
```

Move only type.

---

### 🧹 Destructor

Resets the terminal back to the normal screen buffer.

---

### `Terminal::changes`

```cpp
ScreenBuffer changes;  // write to this
```

A buffer of the 'staged changes' to commit to the screen later.

### `Terminal::event_queue`

```cpp
static EventQueue event_queue; // read from this
```

The queue of user input events.

### `Terminal::foreground` `Terminal::background`

```cpp
Color foreground = TermColor::Default;
Color background = TermColor::Default;
```

The Colors used when `TermColor::Default` is used. These are the 'default' colors for
the entire application, this is what `Brush` defaults to.

### `Terminal::cursor`

```cpp
using Cursor = std::optional<Point>;
Cursor cursor{std::nullopt};
```

The current cursor location, if any. Top-left is `Point{0, 0}`.

---

### `Terminal::commit_changes`

```cpp
void commit_changes();
```

Write the `changes` to the actual terminal screen and reset `changes` to default state.

---

### `Terminal::run_read_loop`

```cpp
void run_read_loop(std::stop_token st);
```

Runs a loop that reads input from the terminal and appends it to the `event_queue`.
Exits when the `stop_token` is `stop_requested()`.

---

### `Terminal::size`

Returns the current size of the terminal screen.

---

</details>

## 🧩 ox::Canvas

[`#include <ox/core/terminal.hpp>`](../include/ox/core/terminal.hpp)

`struct Canvas;`

A sub-region 'view' of a ScreenBuffer at some offset and bound to some size. This
provides access to the underlying Glyphs of the buffer without having to do offset math.
Because it is a view type, Canvas objects can be copied and any modifications will be
done to the same underlying ScreenBuffer.

```cpp
void paint(Canvas c) override {
    c[{0, 0}] = U'X' | fg(XColor::Blue);
    c[{.x = 6, .y = 10}].symbol = U'X';
}
```

<details>
<summary><strong>Details</strong></summary>

### 🏗️ Constructors

Aggregate Constructor, not copy or move assignable because of ScreenBuffer reference
member.

### Public Objects

```cpp
ScreenBuffer& buffer;
Point at;
Area size;
```

### `Canvas::operator[]`

```cpp
auto operator[](Point p) -> Glyph&;
auto operator[](Point p) const -> Glyph const&;
```

Provides access to a Glyph on the underlying ScreenBuffer. The Glyph is found by
offsetting the given Point by `Canvas::at`and then accessing that Point in the held
ScreenBuffer reference.

A Canvas starts at the top left with `Point{.x = 0, .y = 0}` and ends at the bottom
right with `Point{.x = size.width - 1, .y = size.height - 1}`.

Undefined behavior if Point `p` is out of bounds of the Canvas' dimensions.

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
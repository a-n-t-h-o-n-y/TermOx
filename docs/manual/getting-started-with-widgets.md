# Getting Started with Widgets

We'll begin with the simplest use of this library, a lone Widget that paints
some text to the screen:

```cpp
#include <termox/termox.hpp>
using namespace ox;

class App : public Widget {
    auto paint_event() -> bool override
    {
        Painter{*this}.put("Hello, World!", {10, 5});
        return Widget::paint_event();
    }
};

int main() { return System{}.run<App>(); }
```

This program will paint the text `Hello, World!` to the screen at coordinates
`{10, 5}`. We can expand on this to add color and traits to the text:

```cpp
#include <termox/termox.hpp>
using namespace ox;

class App : public Widget {
    auto paint_event() -> bool override
    {
        auto text = L"╔═Hello═World═╝" | fg(Color::Green) | Trait::Bold;
        auto p    = Painter{*this};
        p.put(text, {10, 5});
        return Widget::paint_event();
    }
};

int main() { return System{}.run<App>(); }
```

This code will now print a bold `╔═Hello═World═╝` with green foreground
starting at 10 cell positions to the right and 5 cell positions down from the
top left of the terminal screen.

## Breakdown

```cpp
#include <termox/termox.hpp>
using namespace ox;
```

Make all `ox` namespace names accessible. The `ox` names used in this example
are: `Widget`, `Painter`, `System`, `Color`, and `Trait`.

---

```cpp
class App : public Widget {
```

Inherit from `ox::Widget`, creating a new Widget type.

---

```cpp
 auto paint_event() -> bool override
```

Override the `paint_event` handler. This method is called every time the Widget
needs to be re-painted, it is one of many event handlers that Widget has. The
library internals take care of calling this method when needed.

---

```cpp
auto text = L"╔═Hello═World═╝" | fg(Color::Green) | Trait::Bold;
```

Create a [`Glyph_string`](glyph-string.md) with a foreground [Color](colors.md)
and bold [Trait](traits.md). Traits and Colors are applied with the pipe
operator, a wide string literal is used for the text.

---

```cpp
auto p    = Painter{*this};
p.put(text, {10, 5});
```

Create a `Painter` object to put [`Glyphs`](glyph.md) to the screen. The `put`
method takes a `Glyph` or `Glyph_string` and a [`Point`](point.md). The
coordinates are local to the Widget, with the origin at the top left, x extends
to the right with positive values, and y extends downward with positive values.
Each `paint_event` call begins with a blank display.

---

```cpp
return Widget::paint_event();
```

Call down to the base class' implementation, this is common practice for all
event handlers in TermOx.

---

```cpp
int main() { return System{}.run<App>(); }
```

Run the library with your application as the head Widget, forwarding all user
input as events to your Widget.

---

An entire application could be built within a single Widget by painting
everything needed in the single `paint_event` method, and overriding other
needed event handlers. This would quickly become overly complex for most apps.

Widgets perform best when they have a single, well-defined purpose.
[Layouts](layouts.md) can be used to arrange the pieces of your application, and
[Signals](signals.md) to communicate between Widgets.

Most applications only need a handful of custom Widgets, the [Widget
Library](widget.md#widget-library) provides a collection of common UI elements
for re-use.

## See Next

- [Building with Existing Widgets](building-with-existing-widgets.md)
- [Creating New Widgets](creating-new-widgets.md)
- [`main()` Function](main-function.md)
- [Quick Prototyping](quick-prototyping.md)

## Previous

- [Getting Started](getting-started.md)

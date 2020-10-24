# Getting Started with Widgets

We'll begin with the simplest use of this library, a lone Widget that paints
some text to the screen:

```cpp
#include <cppurses/cppurses.hpp>

class My_application : public cppurses::Widget {
   protected:
    auto paint_event() -> bool override
    {
        using namespace cppurses;
        auto text = L"╔═Hello═World═╝" | foreground(Color::Green) | Trait::Bold;
        auto p    = Painter{*this};
        p.put(text, {10, 5});
        return Widget::paint_event();
    }
};

int main() { return cppurses::System{}.run<My_application>(); }
```

This piece of code will print a bold `╔═Hello═World═╝` with green foreground
starting at 10 cell positions to the right and 5 cell positions down from the
top left of the terminal screen.

## Breakdown

```cpp
#include <cppurses/cppurses.hpp>
```

Make all `cppurses` namespace names accessible.

---

```cpp
class My_application : public cppurses::Widget {
```

Inherit from `cppurses::Widget`, creating a new Widget type.

---

```cpp
protected:
 auto paint_event() -> bool override
```

Override the paint_event handler. This method is called every time the Widget
needs to be re-painted, it is one of many event handlers that `Widget` has. The
library internals take care of calling this method when needed.

---

```cpp
auto text = L"╔═Hello═World═╝" | foreground(Color::Green) | Trait::Bold;
```

Create a [Glyph_string](glyph-string.md) with a foreground [Color](colors.md)
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
event handlers in CPPurses.

---

```cpp
int main() { return cppurses::System{}.run<My_application>(); }
```

Run the library with your application as the head Widget, forwarding all user
input as events to your Widget.

---

An entire application could be built within a single Widget, by drawing the
entire application in the sole `paint_event` method, and overriding other needed
event handlers. This would quickly become overly complex and cumbersome for most
apps.

Widgets perform best when they have a single, well-defined purpose.
[Layouts](layouts.md) may then be employed to arrange the pieces of your
application, and [Signals](signals.md) to communicate between Widgets.

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

# `main()` Function

The `main()` function's role in TermOx is to initialize the terminal screen, set
any global options that `Terminal` provides, set the head widget, then run the
main event loop.

The simplest form looks something like this

```cpp
#include <termox/termox.hpp>

int main() { return ox::System{}.run<App_widget>(); }
```

This constructs the `System` object, creates an instance of the `App_widget` and
sets it as the head Widget, it then runs the event loop.

If you need to change any Terminal options, or construct a widget and manually
set it as the head Widget, you'll need to spread out the above calls.

```cpp
#include <termox/termox.hpp>

int main()
{
    ox::System sys;
    sys.terminal.initialize();

    // Make any calls to Terminal functions, such as setting the color palette,
    // or put the terminal into raw mode.

    App_widget app;
    app.foo();
    sys.set_head(&app);
    return sys.run();
}
```

## Next

- [Quick Prototyping](quick-prototyping.md)

## Previous

- [Creating New Widgets](creating-new-widgets.md)

# Signals

Signals are used to communicate between Widgets - it's how one Widget can react
to another Widget's actions. For instance, a Button press can trigger some state
change in another Widget, and the Button doesn't have to know anything about the
other Widget to do this.

## Basic Use

A Signal can be connected to any number of Slots(callback functions). When a
Signal is 'emitted'(with the call operator), all connected Slots are invoked
with the same parameter that the Signal was emitted with.

```cpp
// Create a Signal
auto activated = sl::Signal<void(char)>{};

// Connect a Slot/Function to the Signal
auto print = [](char c){ std::cout << c; };
activated.connect(print);

// Emit the Signal
activated('a');
```

## Use in Layouts

It's common to make connections to Signals within Layout constructors since
there is direct access to all child Widgets that may need to communicate with
each other.

```cpp
struct Foo : layout::Vertical<> {
    Textbox& text = this->make_child<Textbox>();
    Button& btn   = this->make_child<Button>("Push Me");

    Foo() { btn.pressed.connect([this]{ text.append("Button Pressed\n"); }); }
};
```

## Signal Aliases

If you find yourself accessing a Signal contained in a Widget, contained in a
Layout, contained in another Layout, etc... Then you might try creating a Signal
Alias closer to the use site. This Alias can be a reference to the original
Signal, but held higher up in the Layout structure. This also gives you the
chance to rename the Signal to something specific to your use case, giving more
readable code at the connection site.

```cpp
struct Foo_bar : layout::Horizontal<Button> {
    Button& foo_btn = this->make_child("Do Foo");
    Button& bar_btn = this->make_child("Do Bar");

    sl::Signal<void()>& foo = foo_btn.pressed;
    sl::Signal<void()>& bar = bar_btn.pressed;
};

struct App : layout::Vertical<> {
    Foo_bar& fb        = this->make_child<Foo_bar>();
    Status_bar& status = this->make_child<Status_bar>();

    App()
    {
        fb.foo.connect([]{ do_foo(); });
        fb.foo.connect([this]{ status.update_status("Foo Happened"); });

        fb.bar.connect([]{ do_bar(); });
        fb.bar.connect([this]{ status.update_status("Bar Happened"); });
    }
};
```

The full Signals Library and documentation can be found
[here](https://github.com/a-n-t-h-o-n-y/signals-light).

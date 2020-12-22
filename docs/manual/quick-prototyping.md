# Quick Prototyping

Each Widget and Layout type in the Library has a free function with the same
name as the class, but in lowercase. This function will return a newly created
Widget or Layout, wrapped in a `std::unique_ptr`. Each of these functions will
forward its parameters to the constructor of the named type.

Layouts can be constructed from a list of `std::unique_ptr` to their Widget
type, this allows for concise source code describing the user interface.

```cpp
using namespace ox;
using namespace ox::pipe;

auto useless_widgets() -> std::unique_ptr<layout::Horizontal<>>
{
    return
        layout::horizontal
        (
            titlebar(L"Useless Widgets"),
            labeled_checkbox(L"Check Me! | Trait::Bold"),
            layout::horizontal
            (
                button(L"Press Me!") | bg(Color::Light_blue) | fixed_width(11),
                textbox(L"Edit Me!"),
            )
        );
}
```

As seen above, any [pipe](pipe.md) namespace function can be used to change
properties inline with the creation of the Widget.

Creating Widgets in this style can be useful to test out new designs with
layouts, colors, size policies, etc...

## Connecting Slots to Signals

This style does have drawbacks; the new Widget cannot have its own signals as
members, since a new type is not created, and [signals](signals.md) cannot be
easily connected between sibling Widgets.

This makes it best for self contained Widget collections that do not need to
emit signals, or for prototyping a design.

Connecting signals can be done after the entire Widget is constructed by using
the `Layout::find_child_by_name(...)` method, but only if the child Widget has
been given a name.

```cpp
auto read_write_textbox() -> std::unique_ptr<layout::Vertical<>>
{
    using namespace ox;
    using namespace ox::pipe;

    // Create Widget
    auto w = layout::vertical
    (
        textbox(L"Edit Me!") | bg(Color::Light_green) | name("textbox"),
        layout::horizontal
        (
            read_file()  | name("read-btn"),
            write_file() | name("write-btn")
        ) | fixed_height(1) | name("btn-container")
    );

// TODO verify, also find_by_name returns a pointer, it'll need to be casted,
really not useful unless you have an example where you know the type. or where
you don't need useful connections, maybe change widget features like border or
background color.
    // Connect Signals
    auto* textbox = w->find_child_by_name("textbox");
    auto* btns    = w->find_child_by_name("btn-container");
    btns.find_child_by_name("read-btn") | on_read([&](auto& fs) {
        auto buffer = std::stringstream{};
        buffer << fs.rdbuf();
        textbox.set_contents(buffer.str());
        });
    btns.find_child_by_name("write-btn") | on_write([&](auto& fs) {
        fs << textbox.contents();
        });
    return std::move(w);
}
```

Here, `pipe::name(...)` is used to name each child Widget. Later, each child is
accessed via the `Layout::find_child_by_name(...)` method.

## Next

- [Widget Library](widget.md#widget-library)

## Previous

- [`main()` Function](main-function.md)

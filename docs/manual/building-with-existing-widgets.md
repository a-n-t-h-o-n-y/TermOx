# Building with Existing Widgets

## Modifying an Existing Widget

```cpp
class Thin_button : public ox::Button {
   public:
    Thin_button(ox::Glyph_string title) : Button{std::move(title)}
    {
        using namespace ox::pipe;
        *this | fixed_height(1) | wallpaper(L'~') | bg(ox::Color::Red);
    }
};
```

Small modifications to existing Widgets can be made with public inheritance. In
this example a [`Button`](widgets/button.md) is modified to have a height of one
cell, a [wallpaper](wallpaper.md) of tildes, and a red background. The
constructor is a good place to modify Widget properties, such as:

- [Focus Policy](focus-policy.md)
- [Size Policy](size-policy.md)
- [Signals](signals.md)
- [Brush](brush.md)
- [Border](border.md)
- [Cursor](cursor.md)

The [`pipe`](pipe.md) namespace is being used here, rather than direct member
function calls.

## Combining Widgets with Layouts

```cpp
using namespace ox;

struct Settings_pane : layout::Vertical<> {
    Label& title          = this->make_child<Label>(L"Settings");
    HCheckbox_label& foo = this->make_child<HCheckbox_label>({L"Foo"});
    HCheckbox_label& bar = this->make_child<HCheckbox_label>({L"Bar"});
    Thin_button& baz      = this->make_child<Thin_button>(L"Baz");
    Confirm_button& reset = this->make_child<Confirm_button>(L"Reset");

    Settings_pane() { *this | pipe::fixed_width(16); }
};

struct App : layout::Horizontal<> {
    Settings_pane& settings      = this->make_child<Settings_pane>();
    Main_app_widget& main_widget = this->make_child<Main_app_widget>();
};
```

A Layout is a container for Widgets. The `make_child` template function will
construct the given Widget type, forwarding any parameters to the constructor.
It will append that object to the Layout's child vector and return a reference
to the newly constructed object.

Layouts are parameterized on Widget type, so that it can only hold children of
that type or sub-types of that type. The default is `Widget`, so the template
parameter of `layout::Vertical<>`, above, is left empty. The parameterized type
is the type that `Layout::get_children()` holds references to. `make_child` also
has its template parameter defaulted to the Layout's template parameter.

Layouts can modify their children, just like in the `Thin_button` example above.

Layouts are Widgets, but they should not have to override any event handlers,
if new event handlers are needed, then a new Layout type should be created.

## Next

- [Creating New Widgets](creating-new-widgets.md)
- [`main()` Function](main-function.md)
- [Quick Prototyping](quick-prototyping.md)

## Previous

- [Getting Started with Widgets](getting-started-with-widgets.md)

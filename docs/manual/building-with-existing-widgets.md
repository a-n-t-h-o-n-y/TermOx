# Building with Existing Widgets

CPPurses applications are built up from collections of Widgets, these
collections are called [Layouts](layout.md), and are themselves Widgets. This
mimics the composite pattern, where the application ends up as a tree of
Widgets, with a sigle 'head' Widget at the top level.

A common pattern is to wrap the desired Layout type in a new class, via public
inheritance, then child Widgets can be added via the
`Widget::make_child<Widget_t>(...)` template member function. This function
constructs a new Widget, forwarding any parameters to the constructor, adds it
as a child of the Layout, and returns a reference to the newly constructed
widget. Widgets are added to the layout in the order of the `make_child` calls,
for a `layout::Vertical`, this means the first child Widget occupies the
top-most spot, and the last child Widget added will be at the bottom.

Constructors can be used to modify the look and behavior of the Layout, and of
any direct child Widget with any of:
- [Focus Policy](focus-policy.md)
- [Size Policy](size-policy.md)
- [Signals](signals.md)
- [Brush](brush.md)
- Any Widget specific attributes or behaviors

It is recommended to build up smaller Layout interfaces and not to attempt to
manage any Widget that it does not directly own, such as a child's child Widget.

Layouts should not have to override any event handlers, that points to needing a
new Layout class.

Any Layout type is actually a class template. The single template parameter is
the Widget type that will be held by the collection. The default is `Widget`,
this allows for any type publicly derived from `Widget` to be held by the
Layout, but access to children within the Layout will return only a `Widget`
reference, and not the full type. It is usually best to make this type as
specific as possible, so that there is access to a more concrete type, if
needed.  This isn't always possible, but when a uniform layout of Widget types
is held, this can stop use of `dynamic_cast` hacks, and make the code less
verbose; `Widget::make_child<Widget_t>()` will have its template parameter
defaulted to the Layout's Widget type, and there is a `Child_t` type alias
within `Layout` that can be used to refer to the held Widget type.

```cpp
struct Read_write_btns : cppurses::layout::Horizontal<> {
    Read_write_btns() { *this | cppurses::pipe::fixed_height(1); }

    cppurses::Read_file& read_file   = this->make_child<cppurses::Read_file>();
    cppurses::Write_file& write_file = this->make_child<cppurses::Write_file>();
};

struct Edit_app : cppurses::layout::Vertical<> {
    Edit_app()
    {
        textbox | cppurses::pipe::bg(cppurses::Color::Light_green);
        rw_btns.read_file | cppurses::pipe::on_read([this](auto& fs) {
            auto buffer = std::stringstream{};
            buffer << fs.rdbuf();
            textbox.set_contents(buffer.str());
        });
        rw_btns.write_file | cppurses::pipe::on_write([this](auto& fs) { fs << textbox.contents(); });
    }

    cppurses::Titlebar& titlebar = this->make_child<cppurses::Titlebar>("Edit");
    cppurses::Textbox& textbox   = this->make_child<cppurses::Textbox>("EditMe");
    Read_write_btns& rw_btns     = this->make_child<Read_write_btns>();
};

int main() { return cppurses::System{}.run<Edit_app>(); }
```

There is a second way to build widgets, this is with the `make<Widget_t>(...)`
function. This allows the creation of anonymous Widgets, useful mostly for
prototyping as the syntax is less verbose, but the end product is not easily
reusable in the same way a class name is.

The `make<Widget_t>(...)` method takes constructor parameters for the given
type, and returns an owning `std::unique_ptr` to the created Widget. Layouts can
use a list of these pointers to add them as children.

The pipe operator can be used to modify a Widget in-line with its `make` call.

After the Widget tree is built up, it can be queried for specific Widgets by
name, since Signals usually need to reference siblings, this is the only way to
make those connections.

```cpp
int main() {
    using namespace cppurses;
    using namespace cppurses::pipe;

    // Create Widgets Within Layouts
    auto head_widget =
        make<layout::Vertical<>>
        (
            make<Titlebar>("Edit"),
            make<Textbox>("EditMe") | bg(Color::Light_green) | name("textbox"),
            make<layout::Horizontal>
            (
                make<Read_file>() | name("read-btn"),
                make<Write_file>() | name("write-btn")
            )
        );

    // Hook Up Signals
    auto& textbox = head_widget.find("textbox");
    head_widget.find("read-btn") | on_read([&](auto& fs) {
        auto buffer = std::stringstream{};
        buffer << fs.rdbuf();
        textbox.set_contents(buffer.str());
        });
    head_widget.find("write-btn") | on_write([&](auto& fs) { fs << textbox.contents(); });

    return System{}.run(*head_widget);
}
```

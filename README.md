[![Build Status](https://github.com/a-n-t-h-o-n-y/TermOx/workflows/build/badge.svg)](https://github.com/a-n-t-h-o-n-y/TermOx/actions?query=workflow%3Abuild)
[![Read the Manual](https://img.shields.io/badge/-Manual-yellow.svg)](docs/manual/index.md)
[![Read the Reference](https://img.shields.io/badge/-API%20Reference-blue.svg)](https://a-n-t-h-o-n-y.github.io/TermOx/hierarchy.html)

# TermOx 🐂

**TermOx** is a Terminal User Interface(TUI) Framework written in  C++17. Built
on top of [Escape](https://github.com/a-n-t-h-o-n-y/Escape), it defines a set of
Widgets, Layouts, and Events that make it quick to craft unique user interfaces
in the terminal.

## Colors

<p align="center">
  <img src="docs/images/colors.png">
</p>

**TermOx Color Palettes** support full true color with RGB and HSL values,
[XTerm 256 color indices](https://jonasjacek.github.io/colors/), and Dynamic
Colors.

Color Palettes are used to group Color Definitions together and assign names to
each color. Palettes can be changed at runtime to give applications a simple way
to set up color schemes.

Dynamic Colors are a special type of Color Definition that is updated to a new
value at a given time interval. TermOx comes with Rainbow and Fade Dynamic Color
types, and various modulation shapes to smoothly transition between the colors
in time.

Not all terminals support true color or the complete 256 colors defined by
XTerm.

## Interactivity

<p align="center">
  <img src="docs/images/interactive.png">
</p>

Mouse and Keyboard input is fully supported. Widgets are able to handle Events
sent to them, update their state and re-paint themselves.

Mouse input supports up to 11 mouse buttons(including scrolling), modifier
keys(`shift`, `ctrl`, `alt`), mouse click and drag, and always-on mouse
movement.

Keyboard input supports all printable and non-printable keyboard/keypad presses
and modifier keys(`shift`, `ctrl`, `alt`, `meta`).  Signals generated by special
key presses(`Ctrl+c`, `Ctrl+z`, etc...) can be supressed by TermOx and sent to
Widgets as key combinations.

Most(but not all) terminals support mouse movement. Some terminals intercept
modifier keys for their own purposes.

## Animation

<p align="center">
  <img src="docs/images/animation.gif">
</p>

Each Widget can enable Animation for itself. This will send Timer Events to the
given Widget at a specified time interval. The Widget can handle these Events
with a virtual function override for Timer Events and update its state and
re-paint itself.

Individual color definitions can be animated with Dynamic Colors.

## Widget Library

<p align="center">
  <img src="docs/images/library.png">
</p>

The [Widget Library](docs/manual/widget.md#widget-library) contains many generic
Widget and Layout types common to many applications. Often times they are the
building blocks that form a significant portion of a TUI.

Widgets in the library are written in a way that attempts to be generic enough
for most uses. Many variations on the same Widget are possible with template
parameters and type aliases spelling out useful combinations as distinct Widget
types.

## Layouts

<p align="center">
  <img src="docs/images/combinations.png">
</p>

Widgets are glued together via Layouts, the three Layout types provided by
TermOx are `Vertical`, `Horizontal`, and `Stack`. Layouts are themselves Widgets
that are able to hold other Widgets, they provide the logic for enabling,
resizing, and placement of each of their child Widgets.

Patterns of Layout use are encapsulated by the generic `Pair`, `Tuple`, and
`Array` utility types; combining Widgets is as simple as listing Widget types,
or the number of Widgets of specific type, as in the case of `Array`.

## Custom Widgets

<p align="center">
  <img src="docs/images/custom.png">
</p>

Besides the provided Widget Library, anyone can create new Widgets from scratch.
By inheriting from the `Widget` class, you are provided with virtual function
Event Handlers. These allow handling of everything from user input, animation,
painting to the screen, focus in/out, and others. Existing Widget types can be
inherited from in order to build on top of existing behavior.

## Signals

TermOx uses the [Signals Light](https://github.com/a-n-t-h-o-n-y/signals-light)
library to provide Widget to Widget communication. Widgets can add Signals as
class members, a Signal can be emitted anytime by its Widget, and all registered
observers of the Signal will be notified along with any data sent when emitted.

## Build Instructions

TermOx depends on the [Signals
Light](https://github.com/a-n-t-h-o-n-y/signals-light) and
[Escape](https://github.com/a-n-t-h-o-n-y/Escape) libraries, these are both
included as git submodules.

    git clone https://github.com/a-n-t-h-o-n-y/TermOx.git
    mkdir TermOx/build && cd TermOx/build
    git submodule update --init --recursive   # Pull in dependencies
    cmake .. -DCMAKE_BUILD_TYPE=Release       # Generate Makefiles
    make                                      # Build library
    make demos                                # Build demos(optional)
    make termox.unit.tests                    # Build Unit Tests(optional)
    make termox.ui.tests                      # Build UI Tests(optional)
    make install                              # Install to system directories(optional)

## Using the Library

It is recommended to clone this library into your project and use it as a
submodule rather than installing it to your system. This way you can lock your
application to a specific version of TermOx.

CMake will add the library target `TermOx` if you add the TermOx directory
with `add_subdirectory(...)`.

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.2)
add_executable(my_app
               ...
               my_app.main.cpp
)

# If TermOx is cloned into a directory named external/
add_subdirectory(external/TermOx)
target_link_libraries(my_app TermOx)
```

## Code Example

### Custom Widget

The following code demonstrates how to create a new Widget type from scratch. It
is a simple `Pinbox` Widget that lets a user place pins of various colors on a
dark background, much like a Lite-Brite.

```cpp
#include <map>
#include <string>

#include <termox/termox.hpp>

class Pinbox : public ox::Widget {
   public:
    // Emitted when a new pin is inserted.
    sl::Signal<void(ox::Point)> pin_inserted;

    // Emitted when an existing pin is removed.
    sl::Signal<void(ox::Point)> pin_removed;

   public:
    // Set the Color to be used for new pins inserted.
    void set_foreground(ox::Color c) { foreground_ = c; }

    // Remove all pins from the screen.
    void clear_screen() { points_.clear(); this->update(); }

   protected:
    auto paint_event(ox::Painter& p) -> bool override
    {
        for (auto [xy, color] : points_)
            p.put(U'•' | fg(color), xy);
        return Widget::paint_event(p);
    }

    auto mouse_press_event(ox::Mouse const& m) -> bool override
    {
        this->handle_mouse(m); return Widget::mouse_press_event(m);
    }

    auto mouse_move_event(ox::Mouse const& m) -> bool override
    {
        this->handle_mouse(m); return Widget::mouse_move_event(m);
    }

   private:
    // Inserts pin at Point p, if p is empty. Emits pin_inserted Signal.
    void insert_pin(ox::Point p)
    {
        auto const [_, inserted] = points_.insert({p, foreground_});
        if (inserted) {
            pin_inserted.emit(p);
            this->update();  // Post a Paint Event
        }
    }

    // Removes pin at Point p, if it exists. Emits pin_removed Signal.
    void remove_pin(ox::Point p)
    {
        auto const count = points_.erase(p);
        if (count != 0) {
            pin_removed.emit(p);
            this->update();  // Post a Paint Event
        }
    }

    void handle_mouse(ox::Mouse const& m)
    {
        switch (m.button) {
            case ox::Mouse::Button::Left: this->insert_pin(m.at); break;
            case ox::Mouse::Button::Right: this->remove_pin(m.at); break;
            default: break;
        }
    }

   private:
    std::map<ox::Point, ox::Color> points_;
    ox::Color foreground_ = ox::Color::Light_blue;
};

```

There are two ways of building applications in Termox; with types or values. The
next two sections demonstrate building up an application around the `Pinbox`
Widget. Each section produces the exact same user interface.

### Building with Types

This first section creates new Widget types to build up the interface.

```cpp
class Side_pane : public ox::VTuple<ox::HLabel,
                                    ox::Color_select,
                                    ox::HLabel,
                                    ox::Text_view,
                                    ox::HLine,
                                    ox::HPair<ox::HLabel, ox::Int_view>,
                                    ox::HLine,
                                    ox::Confirm_button,
                                    ox::Widget> {
   public:
    ox::HLabel& color_label        = this->get<0>();
    ox::Color_select& color_select = this->get<1>();
    ox::HLabel& status_label       = this->get<2>();
    ox::Text_view& status_box      = this->get<3>();
    ox::HLabel& count_label        = this->get<5>().first;
    ox::Int_view& count_box        = this->get<5>().second;
    ox::Confirm_button& clear_btn  = this->get<7>();
    ox::Widget& empty_space        = this->get<8>();

   public:
    Side_pane()
    {
        using namespace ox::pipe;

        *this | fixed_width(17) | west_border();
        color_label | align_center() | text("- Color -" | ox::Trait::Bold);
        color_select | fixed_height(2);
        status_label | text("Status" | ox::Trait::Bold);
        status_box | fixed_height(1) | bg(ox::Color::Dark_blue);
        this->get<5>() | fixed_height(1);
        count_label | text("Pin Count" | ox::Trait::Bold) | fixed_width(10);
        count_box | value(0);
        clear_btn.main_btn | text("Clear") | bg(ox::Color::Dark_blue);
    }
};

struct Pinbox_app : ox::HPair<Pinbox, Side_pane> {
    Pinbox& pinbox       = this->first;
    Side_pane& side_pane = this->second;

    Pinbox_app()
    {
        using namespace ox::pipe;
        using std::to_string;
        auto& status_box = side_pane.status_box;
        auto& count_box  = side_pane.count_box;

        side_pane.color_select.color_selected.connect([&](ox::Color c) {
            pinbox.set_foreground(c);
            status_box | text(ox::color_to_string(c) + " Set");
        });

        side_pane.clear_btn.pressed.connect([&] {
            pinbox.clear_screen();
            count_box | value(0);
            status_box | text("Screen Cleared");
        });

        pinbox.pin_inserted.connect([&](ox::Point at) {
            count_box | value(count_box.value() + 1);
            status_box |
                text("Added x" + to_string(at.x) + " y" + to_string(at.y));
        });

        pinbox.pin_removed.connect([&](ox::Point at) {
            count_box | value(count_box.value() - 1);
            status_box |
                text("Removed x" + to_string(at.x) + " y" + to_string(at.y));
        });
    }
};

int main() { return ox::System{ox::Mouse_mode::Drag}.run<Pinbox_app>(); }
```

### Building with Values

This next section produces the same interface as the one above, but uses
existing Widget objects and modifies them to build up the interface.

```cpp
auto pinbox_app()
{
    using namespace ox::pipe;
    auto pa =
        ox::hpair(
            std::make_unique<Pinbox>(),
            ox::vtuple(
                ox::hlabel("- Color -" | ox::Trait::Bold) | align_center(),
                ox::color_select() | fixed_height(2),
                ox::hlabel("Status" | ox::Trait::Bold),
                ox::text_display() | fixed_height(1) | bg(ox::Color::Dark_blue),
                ox::hline(),
                ox::hpair(
                    ox::hlabel("Pin Count" | ox::Trait::Bold) | fixed_width(10),
                    ox::int_view(0)
                ) | fixed_height(1),
                ox::hline(),
                ox::confirm_button("Clear"),
                ox::widget()
            ) | fixed_width(17) | west_border()
        );

    // Access individual Widgets using the `Pair` and `Tuple` interfaces.
    auto& pinbox       = pa->first;
    auto& color_select = pa->second.get<1>();
    auto& status_box   = pa->second.get<3>();
    auto& count_box    = pa->second.get<5>().second;
    auto& clear_btn    = pa->second.get<7>();

    clear_btn.main_btn | bg(ox::Color::Dark_blue);

    color_select.color_selected.connect([&](ox::Color c) {
        pinbox.set_foreground(c);
        status_box | text(color_to_string(c) + " Set");
    });

    clear_btn.pressed.connect([&] {
        pinbox.clear_screen();
        count_box | value(0);
        status_box | text("Screen Cleared");
    });

    using std::to_string;
    pinbox.pin_inserted.connect([&](ox::Point at) {
        count_box | value(count_box.value() + 1);
        status_box | text("Added x" + to_string(at.x) + " y" + to_string(at.y));
    });

    pinbox.pin_removed.connect([&](ox::Point at) {
        count_box | value(count_box.value() - 1);
        status_box |
            text("Removed x" + to_string(at.x) + " y" + to_string(at.y));
    });

    return pa;
}

int main() { return ox::System{ox::Mouse_mode::Drag}.run(*pinbox_app()); }
```

Although this code is somewhat less verbose than the typed style, it does have
the limitation that it cannot add named data. If you need to hold an `int
count;` variable that multiple Widgets access, you need a new Widget type that
holds that variable.

<p align="center">
  <img src="docs/images/pinbox.png">
</p>

See the [Manual](docs/manual/index.md) for more in depth explanations and
examples.

Reference documentation can be found
[here](https://a-n-t-h-o-n-y.github.io/TermOx/hierarchy.html).

This project was previously named **CPPurses**.

## License

This software is distributed under the [MIT License](LICENSE.txt).

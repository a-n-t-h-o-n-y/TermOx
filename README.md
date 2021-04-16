[![Build Status](https://github.com/a-n-t-h-o-n-y/TermOx/workflows/build/badge.svg)](https://github.com/a-n-t-h-o-n-y/TermOx/actions?query=workflow%3Abuild)
[![Read the Manual](https://img.shields.io/badge/-Manual-yellow.svg)](docs/manual/index.md)
[![Read the Reference](https://img.shields.io/badge/-API%20Reference-blue.svg)](https://a-n-t-h-o-n-y.github.io/TermOx/hierarchy.html)

# TermOx 🐂

**TermOx** is a Terminal User Interface Framework for C++17. Built on top of
[Escape](https://github.com/a-n-t-h-o-n-y/Escape), it defines a set of Widgets,
Layouts, and Events that make it quick to craft unique user interfaces in the
terminal.

## Colors

<p align="center">
  <img src="docs/images/colors.png">
</p>

Xterm color indices, true color(rgb or hsl), dynamic colors.
Not all terminals support true color.

## Widget Library

<p align="center">
  <img src="docs/images/library.png">
</p>

The [Widget Library](docs/manual/widget.md#widget-library) contains many generic
Widget and Layout types that are common to many applications. Often times they
are the building blocks that can be pieced together to form a significant
portion of your UI.

## Combinations

<p align="center">
  <img src="docs/images/combinations.png">
</p>

Pair, Tuple, Array. `H` prefix is Horizontal. `V` prefix is Vertical.
Layouts provide basic structure to glue multiple widgets together. Generic
utilities such as pair and tuple and array simplify patterns of widget
combinations.

## Interactivity

<p align="center">
  <img src="docs/images/interactive.png">
</p>

Mouse input including mouse movement events, keyboard and keyboard modifiers.
Though not all terminals support mouse movement or particular modifier keys.

## Animation

<p align="center">
  <img src="docs/images/animation.gif">
</p>

Widgets register to have time events sent to them which allows them to update
their state and repaint themselves. Also dynamic color definitions.

## Custom Widgets

<p align="center">
  <img src="docs/images/custom.png">
</p>

Allows you to create any interactive visual interface you can imagine on the
terminal, even good for interactive games. Simple as inheriting from widget
class and overriding only the necessary virtual event handlers. One or two
Custom widgets unique to your app, with the widget library filling in the rest.
Event handlers for user input, etc.
Inheritance to customize and expand existing Widget types.

## Signals

Widget to widget communication

## Usage

<!-- TODO this is only one usage, and it isn't the common one, its misleading. -->

```cpp
#include <termox/termox.hpp>

int main()
{
    using namespace ox;
    auto app =
        layout::vertical(
            textbox("Hello, World!" | fg(Color::Red) | Trait::Bold) | pipe::name("tb"),
            layout::horizontal<Button>(
                button("Button 1") | bg(Color::Blue) | pipe::name("b1"),
                button("Button 2") | bg(Color::Violet) | pipe::name("b2")
            ) | pipe::fixed_height(1)
        );
    auto& tb = app->find_child_by_name("tb");
    app->find_child_by_name("b1") | pipe::on_press([&tb]{ tb.append("Button 1 Pressed"); });
    app->find_child_by_name("b2") | pipe::on_press([&tb]{ tb.append("Button 2 Pressed"); });
    return System{}.run(*app);
}
```

See the [Manual](docs/manual/index.md) for more in depth explanations and
examples.

Reference documentation can be found
[here](https://a-n-t-h-o-n-y.github.io/TermOx/hierarchy.html).

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
    make install                              # Install to system directories

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
               my_app.main.cpp
               ...
)

# If TermOx is cloned into a directory named external/
add_subdirectory(external/TermOx)
target_link_libraries(foo TermOx)
```

This project was previously named **CPPurses**.

## License

This software is distributed under the [MIT License](LICENSE.txt).

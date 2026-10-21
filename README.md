# TermOx III 🐂🐂🐂

TermOx is a C++20 library for terminal user interfaces. It includes layouts, text
editors, tables, trees, menus, and custom drawing, with keyboard and mouse input.

[![Explorer with a directory tree, file table, and image preview](docs/img/explorer.svg)](examples/explorer.cpp)

## A counter

```cpp
#include <ox/ox.hpp>

using namespace ox;

int main()
{
    auto root = Column{
        IntegerLabel{{.value = 0}},
        Button{{.label = {.text = "Increment"}}},
    };
    auto& [count, button] = root.children;
    Connection{
        .signal = button.on_activate,
        .slot = [](IntegerLabel& count) { count.set_value(count.value + 1); },
    }(count);

    return Application{root}.run();
}
```

## Build and run Explorer

You need Git, CMake 3.18 or newer, and a C++20 compiler and standard library with
`std::format` and coroutine support. CMake fetches the dependencies on the first configure.

```sh
git clone https://github.com/a-n-t-h-o-n-y/TermOx.git
cd TermOx
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target TermOx.examples.explorer
./build/examples/TermOx.examples.explorer
```

Ctrl-C exits.

## Documentation

- [Getting Started](docs/getting_started.md): create, build, and run your own project.
- [Guides](docs/README.md): core concepts, layout, widgets, and recipes.
- [API reference](https://a-n-t-h-o-n-y.github.io/TermOx/): methods, options, and signals.
- [Apps](examples/README.md): source files and build targets.

See [Contributing](docs/contributing.md) for build and test instructions.
TermOx is released under the [MIT license](LICENSE).

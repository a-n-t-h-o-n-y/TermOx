# Getting Started

All library dependencies are included as git submodules, this includes
[ncurses](https://invisible-island.net/ncurses/) and a
[Signals](https://github.com/a-n-t-h-o-n-y/Signals) library. I recommend using
the library as a git submodule within your own project, importing it with CMake,
then linking the `TermOx` target to your application. This allows you to lock
the version of TermOx for each project you use it with. The API is still
unstable and installing it on your system may make it less easy to manage across
multiple projects.

## Adding the Submodule

```bash
cd project-dir/
mkdir external/
git submodule add https://github.com/a-n-t-h-o-n-y/TermOx.git external/TermOx
git submodule update --init --recursive
```

## Linking to Your Application

```CMake
# CMakeLists.txt
...
add_subdirectory(external/TermOx)

add_executable(hello-world src/main.cpp)
target_link_libraries(hello-world PRIVATE TermOx)
...
```

## Hello World Application

```cpp
// main.cpp
#include <termox/termox.hpp>

int main()
{
    return ox::System{}.run<ox::Textbox>("Hello, World!");
}
```

## Manual Dependency Management

### Library Objects

- `build/external/TermOx/src/libTermOx.a`
- `external/ncurses-snapshots/output/lib/libncursesw.a`
- Also depends on your systems threading library
- May require you to link to
  [`dl`](https://refspecs.linuxbase.org/LSB_3.1.1/LSB-Core-generic/LSB-Core-generic/libdl.html)

### Include Paths

- `external/TermOx/include/`
- `external/TermOx/external/Signals/include/`
- `external/ncurses-snapshots/output/include/`

## Next

- [Getting Started with Widgets](getting-started-with-widgets.md)
- [Building with Existing Widgets](building-with-existing-widgets.md)
- [Creating New Widgets](creating-new-widgets.md)
- [`main()` Function](main-function.md)
- [Quick Prototyping](quick-prototyping.md)

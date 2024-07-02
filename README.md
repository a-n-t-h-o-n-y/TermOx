# TermOx II 🐂🐂

C++20 Terminal User Interface Library

## Build Instructions

**TermOx** depends on the [Signals
Light](https://github.com/a-n-t-h-o-n-y/signals-light) and
[Escape](https://github.com/a-n-t-h-o-n-y/Escape) libraries, these are both
included as git submodules.

    git clone https://github.com/a-n-t-h-o-n-y/TermOx.git
    git submodule update --init --recursive     # Pull in dependencies
    mkdir TermOx/build && cd TermOx/build
    cmake ..
    make TermOx                                 # Build library
    make TermOx.tests.unit                      # Build Unit Tests(optional)

### Enabling Link-Time Optimization (LTO)

For optimized builds, you can enable Link-Time Optimization (LTO) to potentially improve the performance of the final binary. Note that enabling LTO can increase build times. To enable LTO, add `-DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON` to the CMake configuration command:

```bash
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON ..
```
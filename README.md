# TermOx 🐂

Terminal User Interface Library

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
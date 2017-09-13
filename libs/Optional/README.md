## Overview
A header only, C++14 implementation of the Boost::Optional library.

Optional objects wrap any type and allow it to occupy a 'null' state. The
status of the object can be queried with an explicit bool conversion. If true,
the Optional object is valid and can be accessed through a dereference. If
false, the object is in a null(empty) state. Optional objects are stack
allocated.

## Code Example
    opt::Optional<int> opt_int;
    if (opt_int) {
        std::cout << "This is never reached.\n";
    } else {
        *opt_int += 7;
    }

    opt::Optional<std::string> opt_str{"Hello, World!"};
    if (opt_str) {
        std::cout << *opt_str << " is " << opt_str->size() << " bytes long.\n";
    } else {
        std::cout << "This is never reached.\n";
    }

    opt::Optional<char> opt_char{opt::none};
    if (!opt_char) {
        std::cout << "opt_char was initialized empty with opt::none.\n";
    }

## Motivation
This class wrapper is useful when a default constructed object, or 'zero', or
any other valid state is not sufficient to convey the idea of empty, or
non-existant.

This implementation was created for personal practice and for use in
[CPPurses](https://github.com/a-n-t-h-o-n-y/CPPurses).

## Installation
CMakeLists.txt included, it will generate files to install and build tests.

## Documentation
Doxygen documentation can be found [here](
https://a-n-t-h-o-n-y.github.io/Optional/).

## Tests
Optional uses the google testing framework.

## License
This software is distributed under the [MIT License](LICENSE.txt).

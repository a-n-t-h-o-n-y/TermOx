## Overview
A C++14 implementation of the Boost::Signals2 library.

Signals and Slots are a callback system that features parameter passing to
callbacks(Slots), multiple callback targets for a single caller(Signal), and 
object lifetime tracking.

## Code Example
    int foo(char c, double d) { return 7; }

    sig::Slot<int(char, double)> slot_1 = [](char, double) { return 3; };
    sig::Slot<int(char, double)> slot_2{foo};

    auto bar = std::make_shared<int>{5};
    slot_2.track(bar);

    sig::Signal<int(char, double)> signal_1;
    signal_1.connect(slot_1);
    signal_1.connect(slot_2);

    auto optional_result = signal_1('f', 3.2);

    ASSERT_TRUE(optional_result);
    EXPECT_EQ(7, *optional_result);

    bar.reset();
    optional_result = signal_1('g', 7.2);

    ASSERT_TRUE(optional_result);
    EXPECT_EQ(3, *optional_result);


## Motivation
This implementation was created for personal practice. It is
also used by [CPPurses](https://github.com/a-n-t-h-o-n-y/CPPurses).

## Installation
CMakeLists.txt included to generate build files and tests.

## Documentation
Doxygen documentation can be found [here](
https://a-n-t-h-o-n-y.github.io/Signals/).

## Tests
Signals uses google test.

## License
This software is distributed under the [MIT License](LICENSE.txt).

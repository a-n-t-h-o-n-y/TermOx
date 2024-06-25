#pragma once

namespace ox {

/**
 * Implements the 'overload' pattern, useful for creating lambda overload sets.
 */
template <class... Ts>
struct Overload : Ts... {
    using Ts::operator()...;
};

}  // namespace ox
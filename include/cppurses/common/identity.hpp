#ifndef CPPURSES_COMMON_IDENTITY_HPP
#define CPPURSES_COMMON_IDENTITY_HPP
#include <utility>

namespace cppurses {

/// Provides an Identity projection operator(), overloaded for any type.
struct Identity {
    template <typename T>
    constexpr auto operator()(T&& x) const -> T&&
    {
        return std::forward<T>(x);
    }
};

}  // namespace cppurses
#endif  // CPPURSES_COMMON_IDENTITY_HPP

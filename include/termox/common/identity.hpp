#ifndef TERMOX_COMMON_IDENTITY_HPP
#define TERMOX_COMMON_IDENTITY_HPP
#include <utility>

namespace ox {

/// Provides an Identity projection operator(), overloaded for any type.
struct Identity {
    template <typename T>
    constexpr auto operator()(T&& x) const -> T&&
    {
        return std::forward<T>(x);
    }
};

}  // namespace ox
#endif  // TERMOX_COMMON_IDENTITY_HPP
